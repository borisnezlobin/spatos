use core::convert::TryFrom;
#[allow(deprecated)]
use core::hash::{BuildHasherDefault, SipHasher};
use core::str;

use alloc::string::String;

use hashbrown::HashMap;
use redox_initfs::{InitFs, InodeStruct, Inode, InodeDir, InodeKind, types::Timespec};

use syscall::data::{Packet, Stat};
use syscall::error::*;
use syscall::flag::*;
use syscall::scheme::{calc_seek_offset_usize, SchemeMut};

struct Handle {
    inode: Inode,
    seek: usize,
    // TODO: Any better way to implement fpath? Or maybe work around it, e.g. by giving paths such
    // as `initfs:__inodes__/<inode>`?
    filename: String,
}
pub struct InitFsScheme {
    #[allow(deprecated)]
    handles: HashMap<usize, Handle, BuildHasherDefault<SipHasher>>,
    next_id: usize,
    fs: InitFs<'static>,
}
impl InitFsScheme {
    pub fn new(bytes: &'static [u8]) -> Self {
        Self {
            handles: HashMap::default(),
            next_id: 0,
            fs: InitFs::new(bytes).expect("failed to parse initfs"),
        }
    }

    fn get_inode(fs: &InitFs<'static>, inode: Inode) -> Result<InodeStruct<'static>> {
        fs.get_inode(inode).ok_or_else(|| Error::new(EIO))
    }
    fn next_id(&mut self) -> usize {
        assert_ne!(self.next_id, usize::MAX, "usize overflow in initfs scheme");
        self.next_id += 1;
        self.next_id
    }
}


struct Iter {
    dir: InodeDir<'static>,
    idx: u32,
}
impl Iterator for Iter {
    type Item = Result<redox_initfs::Entry<'static>>;

    fn next(&mut self) -> Option<Self::Item> {
        let entry = self.dir.get_entry(self.idx).map_err(|_| Error::new(EIO));
        self.idx += 1;
        entry.transpose()
    }
    fn size_hint(&self) -> (usize, Option<usize>) {
        match self.dir.entry_count().ok() {
            Some(size) => {
                let size = usize::try_from(size).expect("expected u32 to be convertible into usize");
                (size, Some(size))
            }
            None => (0, None),
        }
    }
}

fn inode_len(inode: InodeStruct<'static>) -> Result<usize> {
    Ok(match inode.kind() {
        InodeKind::File(file) => file.data().map_err(|_| Error::new(EIO))?.len(),
        InodeKind::Dir(dir) => (Iter { dir, idx: 0 })
            .fold(0, |len, entry| len + entry.and_then(|entry| entry.name().map_err(|_| Error::new(EIO))).map_or(0, |name| name.len() + 1)),
        InodeKind::Unknown => return Err(Error::new(EIO)),
    })
}

impl SchemeMut for InitFsScheme {
    fn open(&mut self, path: &str, _flags: usize, _uid: u32, _gid: u32) -> Result<usize> {
        let mut components = path
            // trim leading and trailing slash
            .trim_matches('/')
            // divide into components
            .split('/')
            // filter out double slashes (e.g. /usr//bin/...)
            .filter(|c| !c.is_empty());

        let mut current_inode = InitFs::ROOT_INODE;

        while let Some(component) = components.next() {
            match component {
                "." => continue,
                ".." => {
                    let _ = components.next_back();
                    continue
                }

                _ => (),
            }

            let current_inode_struct = Self::get_inode(&self.fs, current_inode)?;

            let dir = match current_inode_struct.kind() {
                InodeKind::Dir(dir) => dir,

                // If we still have more components in the path, and the file tree for that
                // particular branch is not all directories except the last, then that file cannot
                // exist.
                InodeKind::File(_) | InodeKind::Unknown => return Err(Error::new(ENOENT)),
            };

            let mut entries = Iter {
                dir,
                idx: 0,
            };

            current_inode = loop {
                let entry_res = match entries.next() {
                    Some(e) => e,
                    None => return Err(Error::new(ENOENT)),
                };
                let entry = entry_res?;
                let name = entry.name().map_err(|_| Error::new(EIO))?;
                if name == component.as_bytes() {
                    break entry.inode();
                }
            };
        }

        let id = self.next_id();
        let old = self.handles.insert(id, Handle {
            inode: current_inode,
            seek: 0_usize,
            filename: path.into(),
        });
        assert!(old.is_none());

        Ok(id)
    }

    fn read(&mut self, id: usize, mut buffer: &mut [u8]) -> Result<usize> {
        let handle = self.handles.get_mut(&id).ok_or(Error::new(EBADF))?;

        match Self::get_inode(&self.fs, handle.inode)?.kind() {
            InodeKind::Dir(dir) => {
                let mut bytes_read = 0;
                let mut total_to_skip = handle.seek;

                for entry_res in (Iter { dir, idx: 0 }) {
                    let entry = entry_res?;
                    let name = entry.name().map_err(|_| Error::new(EIO))?;

                    let to_skip = core::cmp::min(total_to_skip, name.len() + 1);
                    if to_skip == name.len() + 1 { continue; }

                    let name = &name[to_skip..];

                    let to_copy = core::cmp::min(name.len(), buffer.len());
                    buffer[..to_copy].copy_from_slice(&name[..to_copy]);
                    bytes_read += to_copy;
                    buffer = &mut buffer[to_copy..];

                    if !buffer.is_empty() {
                        buffer[0] = b'\n';
                        bytes_read += 1;
                        buffer = &mut buffer[1..];
                    }

                    total_to_skip -= to_skip;
                }

                handle.seek = handle.seek.saturating_add(bytes_read);

                Ok(bytes_read)
            }
            InodeKind::File(file) => {
                let data = file.data().map_err(|_| Error::new(EIO))?;
                let src_buf = &data[core::cmp::min(handle.seek, data.len())..];

                let to_copy = core::cmp::min(src_buf.len(), buffer.len());
                buffer[..to_copy].copy_from_slice(&src_buf[..to_copy]);

                handle.seek = handle.seek.checked_add(to_copy).ok_or(Error::new(EOVERFLOW))?;

                Ok(to_copy)
            }
            InodeKind::Unknown => return Err(Error::new(EIO)),
        }
    }

    fn seek(&mut self, id: usize, pos: isize, whence: usize) -> Result<isize> {
        let handle = self.handles.get_mut(&id).ok_or(Error::new(EBADF))?;

        let new_offset = calc_seek_offset_usize(handle.seek, pos, whence, inode_len(Self::get_inode(&self.fs, handle.inode)?)?)?;
        handle.seek = new_offset as usize;
        Ok(new_offset)
    }

    fn fcntl(&mut self, id: usize, _cmd: usize, _arg: usize) -> Result<usize> {
        let _handle = self.handles.get(&id).ok_or(Error::new(EBADF))?;

        Ok(0)
    }

    fn fpath(&mut self, id: usize, buf: &mut [u8]) -> Result<usize> {
        let handle = self.handles.get(&id).ok_or(Error::new(EBADF))?;

        // TODO: Copy scheme part in kernel
        let scheme_path = b"initfs:";
        let scheme_bytes = core::cmp::min(scheme_path.len(), buf.len());
        buf[..scheme_bytes].copy_from_slice(&scheme_path[..scheme_bytes]);

        let source = handle.filename.as_bytes();
        let path_bytes = core::cmp::min(buf.len() - scheme_bytes, source.len());
        buf[scheme_bytes..scheme_bytes + path_bytes].copy_from_slice(&source[..path_bytes]);

        Ok(scheme_bytes + path_bytes)
    }

    fn fstat(&mut self, id: usize, stat: &mut Stat) -> Result<usize> {
        let handle = self.handles.get(&id).ok_or(Error::new(EBADF))?;

        let Timespec { sec, nsec } = self.fs.image_creation_time();

        let inode = Self::get_inode(&self.fs, handle.inode)?;

        stat.st_mode = inode.mode() | match inode.kind() { InodeKind::Dir(_) => MODE_DIR, InodeKind::File(_) => MODE_FILE, _ => 0 };
        stat.st_uid = inode.uid();
        stat.st_gid = inode.gid();
        stat.st_size = u64::try_from(inode_len(inode)?).unwrap_or(u64::MAX);

        stat.st_ctime = sec.get();
        stat.st_ctime_nsec = nsec.get();
        stat.st_mtime = sec.get();
        stat.st_mtime_nsec = nsec.get();

        Ok(0)
    }

    fn fsync(&mut self, id: usize) -> Result<usize> {
        if !self.handles.contains_key(&id) {
            return Err(Error::new(EBADF));
        }

        Ok(0)
    }

    fn close(&mut self, id: usize) -> Result<usize> {
        let _ = self.handles.remove(&id).ok_or(Error::new(EBADF))?;
        Ok(0)
    }
}

pub fn run(bytes: &'static [u8], sync_pipe: usize) -> ! {
    let mut scheme = InitFsScheme::new(bytes);

    let socket = syscall::open(":initfs", O_RDWR | O_CLOEXEC | O_CREAT)
        .expect("failed to open initfs scheme socket");

    let _ = syscall::write(sync_pipe, &[0]);
    let _ = syscall::close(sync_pipe);

    let mut packet = Packet::default();

    'packets: loop {
        loop {
            match syscall::read(socket, &mut packet) {
                Ok(0) => break 'packets,
                Ok(_) => break,
                Err(error) if error == Error::new(EINTR) => continue,
                Err(error) => panic!("failed to read from scheme socket: {}", error),
            }
        }
        scheme.handle(&mut packet);
        loop {
            match syscall::write(socket, &packet) {
                Ok(0) => break 'packets,
                Ok(_) => break,
                Err(error) if error == Error::new(EINTR) => continue,
                Err(error) => panic!("failed to write to scheme socket: {}", error),
            }
        }
    }
    syscall::exit(0).expect("initfs: failed to exit");
    unreachable!()
}
