use std::convert::{TryFrom, TryInto};
use std::fs::{DirEntry, File, Metadata, OpenOptions};
use std::io::{prelude::*, SeekFrom};
use std::path::Path;

use std::os::unix::ffi::OsStrExt;
use std::os::unix::fs::{FileExt, FileTypeExt, MetadataExt};

use anyhow::{anyhow, Context, Result};

use redox_initfs::types as initfs;

pub const KIBIBYTE: u64 = 1024;
pub const MEBIBYTE: u64 = KIBIBYTE * 1024;
pub const DEFAULT_MAX_SIZE: u64 = 64 * MEBIBYTE;

enum EntryKind {
    File(File),
    Dir(Dir),
}

struct Entry {
    name: Vec<u8>,
    kind: EntryKind,
    metadata: Metadata,
}
struct Dir {
    entries: Vec<Entry>,
}

struct State<'path> {
    file: OutputImageGuard<'path>,
    offset: u64,
    max_size: u64,
    inode_count: u16,
    buffer: Box<[u8]>,
    inode_table_offset: u32,
}

fn write_all_at(file: &File, buf: &[u8], offset: u64, r#where: &str) -> Result<()> {
    file.write_all_at(buf, offset)?;
    log::trace!("Wrote {}..{} within {}", offset, offset + buf.len() as u64, r#where);
    Ok(())
}

fn read_directory(state: &mut State, path: &Path) -> Result<Dir> {
    let read_dir = path
        .read_dir()
        .with_context(|| anyhow!("failed to read directory `{}`", path.to_string_lossy(),))?;

    let entries = read_dir
        .map(|result| {
            let entry = result.with_context(|| {
                anyhow!(
                    "failed to get a directory entry from `{}`",
                    path.to_string_lossy(),
                )
            })?;

            let metadata = entry.metadata().with_context(|| {
                anyhow!(
                    "failed to get metadata for `{}`",
                    entry.path().to_string_lossy(),
                )
            })?;
            let file_type = metadata.file_type();

            let unsupported_type = |ty: &str, entry: &DirEntry| {
                Err(anyhow!(
                    "failed to include {} at `{}`: not supported by redox-initfs",
                    ty,
                    entry.path().to_string_lossy()
                ))
            };
            let name = entry
                .path()
                .file_name()
                .context("expected path to have a valid filename")?
                .as_bytes()
                .to_owned();

            let entry_kind = if file_type.is_symlink() {
                return unsupported_type("symlink", &entry);
            } else if file_type.is_socket() {
                return unsupported_type("socket", &entry);
            } else if file_type.is_fifo() {
                return unsupported_type("FIFO", &entry);
            } else if file_type.is_block_device() {
                return unsupported_type("block device", &entry);
            } else if file_type.is_char_device() {
                return unsupported_type("character device", &entry);
            } else if file_type.is_file() {
                EntryKind::File(File::open(&entry.path()).with_context(|| {
                    anyhow!("failed to open file `{}`", entry.path().to_string_lossy(),)
                })?)
            } else if file_type.is_dir() {
                EntryKind::Dir(read_directory(state, &entry.path())?)
            } else {
                return Err(anyhow!(
                    "unknown file type at `{}`",
                    entry.path().to_string_lossy()
                ));
            };

            // TODO: Allow the user to specify a lower limit than u16::MAX.
            state.inode_count = state
                .inode_count
                .checked_add(1)
                .ok_or_else(|| anyhow!("exceeded the maximum inode limit"))?;

            Ok(Entry {
                kind: entry_kind,
                metadata,
                name,
            })
        })
        .collect::<Result<Vec<_>>>()?;

    Ok(Dir { entries })
}

fn bump_alloc(state: &mut State, size: u64, why: &str) -> Result<u64> {
    if state.offset + size <= state.max_size {
        let offset = state.offset;
        state.offset += size;
        log::debug!("Allocating range {}..{} in {}", offset, state.offset, why);
        Ok(offset)
    } else {
        Err(anyhow!("bump allocation failed: max limit reached"))
    }
}
struct WriteResult {
    size: u32,
    offset: u32,
}

fn allocate_and_write_file(state: &mut State, mut file: &File) -> Result<WriteResult> {
    let size = file
        .seek(SeekFrom::End(0))
        .context("failed to seek to end")?;

    let size: u32 = size.try_into().context("file too large")?;

    let offset: u32 = bump_alloc(state, size.into(), "allocate space for file")
        .context("failed to allocate space for file")?
        .try_into()
        .context("file offset too high")?;

    let buffer_size: u32 = state.buffer.len().try_into().context("buffer too large")?;

    file.seek(SeekFrom::Start(0))
        .context("failed to seek to start")?;

    let mut relative_offset = 0;

    // TODO: If this would ever turn out to be a bottleneck, then perhaps we could use
    // copy_file_range in `nix`.

    while relative_offset < size {
        let allowed_length = std::cmp::min(buffer_size, size - relative_offset);
        let allowed_length =
            usize::try_from(allowed_length).expect("expected buffer size not to be outside usize");

        file.read(&mut state.buffer[..allowed_length])
            .context("failed to read from source file")?;

        write_all_at(&*state.file, &state.buffer[..allowed_length], u64::from(offset + relative_offset), "allocate_and_write_file buffer chunk")
            .context("failed to write source file into destination image")?;

        relative_offset += buffer_size;
    }

    Ok(WriteResult { size, offset })
}
fn write_inode(
    state: &mut State,
    ty: initfs::InodeType,
    metadata: &Metadata,
    write_result: WriteResult,
    inode: u16,
) -> Result<()> {
    let inode_size: u32 = std::mem::size_of::<initfs::InodeHeader>()
        .try_into()
        .expect("inode header length cannot fit within u32");

    let type_and_mode = ((ty as u32) << initfs::TYPE_SHIFT) | u32::from(metadata.mode() & 0xFFF);

    // TODO: Use main buffer and write in bulk.
    let mut inode_buf = [0_u8; std::mem::size_of::<initfs::InodeHeader>()];

    let inode_hdr = plain::from_mut_bytes::<initfs::InodeHeader>(&mut inode_buf)
        .expect("expected inode struct to have alignment 1, and buffer size to match");

    *inode_hdr = initfs::InodeHeader {
        type_and_mode: type_and_mode.into(),
        length: write_result.size.into(),
        offset: initfs::Offset(write_result.offset.into()),

        gid: 0.into(),//metadata.gid().into(),
        uid: 0.into(),//metadata.uid().into(),
    };

    log::debug!("Writing inode index {} from offset {}", inode, state.inode_table_offset);
    write_all_at(
        &*state.file,
        &inode_buf,
        u64::from(state.inode_table_offset + u32::from(inode) * inode_size),
        "write_inode",
    )
    .context("failed to write inode struct to disk image")
}
fn allocate_and_write_dir(
    state: &mut State,
    dir: &Dir,
    current_inode: &mut u16,
) -> Result<WriteResult> {
    let entry_size =
        u16::try_from(std::mem::size_of::<initfs::DirEntry>()).context("entry size too large")?;
    let entry_count = u16::try_from(dir.entries.len()).context("too many subdirectories")?;

    let entry_table_length = u32::from(entry_count)
        .checked_mul(u32::from(entry_size))
        .ok_or_else(|| anyhow!("entry table length too large when multiplying by size"))?;

    let entry_table_offset: u32 = bump_alloc(state, entry_table_length.into(), "allocate entry table")
        .context("failed to allocate entry table")?
        .try_into()
        .context("directory entries offset too high")?;

    for (index, entry) in dir.entries.iter().enumerate() {
        let (write_result, ty) = match entry.kind {
            EntryKind::Dir(ref subdir) => {
                let write_result =
                    allocate_and_write_dir(state, subdir, current_inode)
                        .with_context(|| {
                            anyhow!(
                                "failed to copy directory entries from `{}` into image",
                                String::from_utf8_lossy(&entry.name)
                            )
                        })?;

                (write_result, initfs::InodeType::Dir)
            }

            EntryKind::File(ref file) => {
                let write_result = allocate_and_write_file(state, file)
                    .context("failed to copy file into image")?;

                (write_result, initfs::InodeType::RegularFile)
            }
        };

        let index: u16 = index
            .try_into()
            .expect("expected dir entry count not to exceed u32");

        *current_inode += 1;
        write_inode(
            state,
            ty,
            &entry.metadata,
            write_result,
            *current_inode,
        )?;

        let (name_offset, name_len) = {
            let name_len: u16 = entry.name.len().try_into().context("file name too long")?;

            let offset: u32 = bump_alloc(state, u64::from(name_len), "allocate file name")
                .context("failed to allocate space for file name")?
                .try_into()
                .context("file name offset too high up")?;

            write_all_at(&*state.file, &entry.name, offset.into(), "writing file name").context("failed to write file name")?;

            (offset, name_len)
        };
        {
            let mut direntry_buf = [0_u8; std::mem::size_of::<initfs::DirEntry>()];

            let direntry = plain::from_mut_bytes::<initfs::DirEntry>(&mut direntry_buf)
                .expect("expected dir entry struct to have alignment 1, and buffer size to match");

            log::debug!("Linking inode {} into dir entry index {}, file name `{}`", current_inode, index, String::from_utf8_lossy(&entry.name));

            *direntry = initfs::DirEntry {
                inode: (*current_inode).into(),
                name_len: name_len.into(),
                name_offset: initfs::Offset(name_offset.into()),
            };

            write_all_at(
                &*state.file,
                &direntry_buf,
                u64::from(entry_table_offset + u32::from(index) * u32::from(entry_size)),
                "allocate_and_write_dir entry",
            )
            .context("failed to write dir entry struct to image")?;
        }
    }

    Ok(WriteResult {
        size: entry_table_length,
        offset: entry_table_offset,
    })
}
fn allocate_contents_and_write_inodes(
    state: &mut State,
    dir: &Dir,
    root_metadata: Metadata,
) -> Result<()> {
    let start_inode = 0;
    let mut current_inode = start_inode;

    let write_result = allocate_and_write_dir(state, dir, &mut current_inode)
        .context("failed to allocate and write all directories and files")?;

    write_inode(
        state,
        initfs::InodeType::Dir,
        &root_metadata,
        write_result,
        start_inode,
    )
}

struct OutputImageGuard<'a> {
    file: File,
    path: &'a Path,
    ok: bool,
}

impl std::ops::Deref for OutputImageGuard<'_> {
    type Target = File;

    fn deref(&self) -> &Self::Target {
        &self.file
    }
}
impl std::ops::DerefMut for OutputImageGuard<'_> {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.file
    }
}

impl Drop for OutputImageGuard<'_> {
    fn drop(&mut self) {
        if !self.ok {
            let _ = std::fs::remove_file(self.path);
        }
    }
}

pub struct Args<'a> {
    pub destination_path: &'a Path,
    pub max_size: u64,
    pub source: &'a Path,
    pub bootstrap_code: Option<&'a Path>,
}
pub fn archive(
    &Args {
        destination_path,
        max_size,
        source,
        bootstrap_code,
    }: &Args,
) -> Result<()> {
    let previous_extension = destination_path.extension().map_or("", |ext| {
        ext.to_str()
            .expect("expected destination path to be valid UTF-8")
    });

    if !destination_path
        .metadata()
        .map_or(true, |metadata| metadata.is_file())
    {
        return Err(anyhow!("Destination file must be a file"));
    }

    let destination_temp_path =
        destination_path.with_extension(format!("{}.partial", previous_extension));

    let destination_temp_file = OpenOptions::new()
        .read(false)
        .write(true)
        .create(true)
        .create_new(false)
        .open(&destination_temp_path)
        .context("failed to open destination file")?;

    let guard = OutputImageGuard {
        file: destination_temp_file,
        path: &destination_temp_path,
        ok: false,
    };

    const BUFFER_SIZE: usize = 8192;

    let mut state = State {
        file: guard,
        offset: 0,
        max_size,
        // Include root directory.
        inode_count: 1,
        buffer: vec![0_u8; BUFFER_SIZE].into_boxed_slice(),
        inode_table_offset: 0,
    };

    let root_path = source;
    let root_metadata = root_path
        .metadata()
        .context("failed to obtain metadata for root")?;
    let root = read_directory(&mut state, root_path).context("failed to read root")?;

    log::debug!("there are {} inodes", state.inode_count);

    // NOTE: The header is always stored at offset zero.
    let header_offset = bump_alloc(&mut state, 4096, "allocate header")?;
    assert_eq!(header_offset, 0);

    let bootstrap_entry = if let Some(bootstrap_code) = bootstrap_code {
        allocate_and_write_file(
            &mut state,
            &File::open(bootstrap_code).with_context(|| {
                anyhow!(
                    "failed to open bootstrap code file `{}`",
                    bootstrap_code.to_string_lossy(),
                )
            })?,
        )?;
        let bootstrap_data = std::fs::read(bootstrap_code).with_context(|| {
            anyhow!(
                "failed to read bootstrap code file `{}`",
                bootstrap_code.to_string_lossy(),
            )
        })?;
        elf_entry(&bootstrap_data)
    } else {
        u64::MAX
    };

    let inode_table_length = {
        let inode_entry_size: u64 = std::mem::size_of::<initfs::InodeHeader>()
            .try_into()
            .expect("expected table entry size to fit");

        inode_entry_size
            .checked_mul(u64::from(state.inode_count))
            .ok_or_else(|| anyhow!("inode table too large"))?
    };

    let inode_table_offset = bump_alloc(&mut state, inode_table_length, "allocate inode table")?;

    // Finally, write the header to the disk image.

    let inode_table_offset = initfs::Offset(
        u32::try_from(inode_table_offset)
            .with_context(|| "inode table located too far away")?
            .into(),
    );

    state.inode_table_offset = inode_table_offset.0.get();

    allocate_contents_and_write_inodes(
        &mut state,
        &root,
        root_metadata,
    )?;

    let current_system_time = std::time::SystemTime::now();

    let time_since_epoch = current_system_time
        .duration_since(std::time::SystemTime::UNIX_EPOCH)
        .context("could not calculate timestamp")?;

    {
        let mut header_bytes = [0_u8; std::mem::size_of::<initfs::Header>()];
        let header = plain::from_mut_bytes(&mut header_bytes)
            .expect("expected header size to be sufficient and alignment to be 1");

        *header = initfs::Header {
            magic: initfs::Magic(initfs::MAGIC),
            creation_time: initfs::Timespec {
                sec: time_since_epoch.as_secs().into(),
                nsec: time_since_epoch.subsec_nanos().into(),
            },
            inode_count: state.inode_count.into(),
            inode_table_offset,
            bootstrap_entry: bootstrap_entry.into(),
            initfs_size: state.file.metadata().context("failed to get initfs size")?.len().into(),
        };
        write_all_at(&*state.file, &header_bytes, header_offset, "writing header")
            .context("failed to write header")?;
    }

    std::fs::rename(&destination_temp_path, destination_path)
        .context("failed to rename output image")?;

    state.file.ok = true;

    Ok(())
}

fn elf_entry(data: &[u8]) -> u64 {
    assert!(&data[..4] == b"\x7FELF");
    match (data[4], data[5]) {
        // 32-bit, little endian
        (1, 1) => u32::from_le_bytes(
            <[u8; 4]>::try_from(&data[0x18..0x18 + 4]).expect("conversion cannot fail"),
        ) as u64,
        // 32-bit, big endian
        (1, 2) => u32::from_be_bytes(
            <[u8; 4]>::try_from(&data[0x18..0x18 + 4]).expect("conversion cannot fail"),
        ) as u64,
        // 64-bit, little endian
        (2, 1) => u64::from_le_bytes(
            <[u8; 8]>::try_from(&data[0x18..0x18 + 8]).expect("conversion cannot fail"),
        ),
        // 64-bit, big endian
        (2, 2) => u64::from_be_bytes(
            <[u8; 8]>::try_from(&data[0x18..0x18 + 8]).expect("conversion cannot fail"),
        ),
        (ei_class, ei_data) => {
            panic!("Unsupported ELF EI_CLASS {} EI_DATA {}", ei_class, ei_data);
        }
    }
}
