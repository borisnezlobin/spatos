use std::collections::BTreeMap;
use std::convert::{TryFrom, TryInto};
use std::os::unix::io::AsRawFd;
use std::{cmp, ops};

use syscall::error::{
    EACCES, EBADF, EBADFD, EEXIST, EINVAL, EIO, EISDIR, ENOMEM, ENOSYS, ENOTDIR, ENOTEMPTY,
    EOVERFLOW,
};
use syscall::flag::{
    O_ACCMODE, O_CREAT, O_DIRECTORY, O_EXCL, O_RDONLY, O_RDWR, O_STAT, O_TRUNC, O_WRONLY,
};
use syscall::{Error, EventFlags, Map, Result, Stat, StatVfs, TimeSpec};
use syscall::{MODE_DIR, MODE_FILE, MODE_PERM, MODE_TYPE, SEEK_CUR, SEEK_END, SEEK_SET};

use redox_scheme::SchemeMut;

use crate::filesystem::{self, DirEntry, File, FileData, Filesystem};

#[derive(Clone)]
struct Handle {
    inode: usize,
    offset: usize,

    opened_as_read: bool,  // opened with O_RDONLY or O_RDWR
    opened_as_write: bool, // opened with O_WRONLY or O_RDWR

    // the three first bits of mode >> 6 if uid matched, mode >> 3 if gid matched, otherwise mode
    // >> 0.
    current_perm: u8,
}

pub struct Scheme {
    scheme_name: String,
    handles: BTreeMap<usize, Handle>,
    next_fd: usize,
    filesystem: Filesystem,
}
impl Scheme {
    /// Create the scheme, with the name being used for `fpath`.
    pub fn new(scheme_name: String) -> Result<Self> {
        Ok(Self {
            scheme_name,
            handles: BTreeMap::new(),
            filesystem: Filesystem::new()?,
            next_fd: 0,
        })
    }
    /// Remove a directory entry, where the entry can be both a file or a directory. Used by both
    /// `unlink` and `rmdir`.
    pub fn remove_dentry(
        &mut self,
        path: &[u8],
        uid: u32,
        gid: u32,
        directory: bool,
    ) -> Result<usize> {
        let removed_entry = {
            let (parent_dir_inode, name_to_delete) =
                self.filesystem.resolve_except_last(path, uid, gid)?;
            let name_to_delete = name_to_delete.ok_or(Error::new(EINVAL))?; // can't remove root
            let parent = self
                .filesystem
                .files
                .get_mut(&parent_dir_inode)
                .ok_or(Error::new(EIO))?;

            let mode = current_perm(parent, uid, gid);
            if mode & 0o2 == 0 {
                return Err(Error::new(EACCES));
            }

            let dentries = parent.data.as_directory_mut().ok_or(Error::new(EBADF))?;

            let (position, entry_inode) = dentries
                .iter()
                .enumerate()
                .find(|(_, d)| d.name == name_to_delete)
                .unwrap();
            let entry_inode = entry_inode.inode;

            let removed_entry = dentries.remove(position);

            if let Some(File {
                data: FileData::Directory(ref data),
                ..
            }) = self.filesystem.files.get(&entry_inode)
            {
                if !directory {
                    return Err(Error::new(EISDIR));
                } else if !data.is_empty() {
                    return Err(Error::new(ENOTEMPTY));
                }
                let parent = self
                    .filesystem
                    .files
                    .get_mut(&parent_dir_inode)
                    .ok_or(Error::new(EIO))?;
                parent.nlink -= 1; // '..' of subdirectory
            }

            removed_entry
        };

        let removed_inode = self
            .filesystem
            .files
            .get_mut(&removed_entry.inode)
            .ok_or(Error::new(EIO))?;

        if let FileData::File(_) = removed_inode.data {
            if directory {
                return Err(Error::new(EISDIR));
            }
            removed_inode.nlink -= 1; // only the parent entry
        } else {
            if !directory {
                return Err(Error::new(ENOTDIR));
            }
            removed_inode.nlink -= 2; // both the parent entry and '.'
        }

        if removed_inode.nlink == 0 && removed_inode.open_handles == 0 {
            self.filesystem.files.remove(&removed_entry.inode);
        }

        Ok(0)
    }

    fn open_existing(&mut self, path: &[u8], flags: usize, uid: u32, gid: u32) -> Result<Handle> {
        let inode = self.filesystem.resolve(path, uid, gid)?;
        let file = self
            .filesystem
            .files
            .get_mut(&inode)
            .ok_or(Error::new(EIO))?;

        if flags & O_STAT == 0 && flags & O_DIRECTORY != 0 && file.mode & MODE_TYPE != MODE_DIR {
            return Err(Error::new(ENOTDIR));
        }

        // Unlike on Linux, which allows directories to be opened without O_DIRECTORY, Redox has no
        // getdents(2) syscall, and thus it adds the additional restriction that directories have
        // to be opened with O_DIRECTORY, if they aren't opened with O_STAT to check whether it's a
        // directory.
        if flags & O_STAT == 0 && flags & O_DIRECTORY == 0 && file.mode & MODE_TYPE == MODE_DIR {
            return Err(Error::new(EISDIR));
        }

        let current_perm = current_perm(file, uid, gid);
        check_permissions(flags, current_perm)?;

        let opened_as_read = flags & O_ACCMODE == O_RDONLY || flags & O_ACCMODE == O_RDWR;
        let opened_as_write = flags & O_ACCMODE == O_WRONLY || flags & O_ACCMODE == O_RDWR;

        if flags & O_TRUNC == O_TRUNC && opened_as_write {
            match file.data {
                // file.data and file.mode should match
                FileData::Directory(_) => return Err(Error::new(EBADFD)),

                // If we opened an existing file with O_CREAT and O_TRUNC
                FileData::File(ref mut data) => data.clear(),
            }
        }

        file.open_handles += 1;

        Ok(Handle {
            inode,
            offset: 0,
            opened_as_read,
            opened_as_write,
            current_perm,
        })
    }
}

impl SchemeMut for Scheme {
    fn open(&mut self, path: &str, flags: usize, uid: u32, gid: u32) -> Result<usize> {
        let exists = self.filesystem.resolve(path.as_bytes(), 0, 0).is_ok();
        if flags & O_CREAT != 0 && flags & O_EXCL != 0 && exists {
            return Err(Error::new(EEXIST));
        }

        let handle = if flags & O_CREAT != 0 && exists {
            self.open_existing(path.as_bytes(), flags, uid, gid)?
        } else if flags & O_CREAT != 0 {
            if flags & O_STAT != 0 {
                return Err(Error::new(EINVAL));
            }

            let (parent_dir_inode, new_name) =
                self.filesystem
                    .resolve_except_last(path.as_bytes(), uid, gid)?;
            let new_name = new_name.ok_or(Error::new(EINVAL))?; // cannot mkdir /

            let current_time = filesystem::current_time();

            let new_inode_number = self.filesystem.next_inode_number()?;

            let mut mode = (flags & 0xFFFF) as u16;

            let new_inode = if flags & O_DIRECTORY != 0 {
                if mode & MODE_TYPE == 0 {
                    mode |= MODE_DIR
                }
                if mode & MODE_TYPE != MODE_DIR {
                    return Err(Error::new(EINVAL));
                }

                File {
                    atime: current_time,
                    crtime: current_time,
                    ctime: current_time,
                    mtime: current_time,
                    gid,
                    uid,
                    ino: new_inode_number,
                    mode,
                    nlink: 2, // parent entry, "."
                    data: FileData::Directory(Vec::new()),
                    open_handles: 1,
                }
            } else {
                if mode & MODE_TYPE == 0 {
                    mode |= MODE_FILE
                }
                if mode & MODE_TYPE == MODE_DIR {
                    return Err(Error::new(EINVAL));
                }

                File {
                    atime: current_time,
                    crtime: current_time,
                    ctime: current_time,
                    mtime: current_time,
                    gid,
                    uid,
                    ino: new_inode_number,
                    mode,
                    nlink: 1,
                    data: FileData::File(Vec::new()),
                    open_handles: 1,
                }
            };
            let current_perm = current_perm(&new_inode, uid, gid);
            check_permissions(flags, current_perm)?;

            self.filesystem.files.insert(new_inode_number, new_inode);

            let parent_file = self
                .filesystem
                .files
                .get_mut(&parent_dir_inode)
                .ok_or(Error::new(EIO))?;
            match parent_file.data {
                FileData::File(_) => return Err(Error::new(EIO)),
                FileData::Directory(ref mut entries) => entries.push(DirEntry {
                    name: new_name.to_owned(),
                    inode: new_inode_number,
                }),
            }

            Handle {
                inode: new_inode_number,
                offset: 0,
                opened_as_read: flags & O_ACCMODE == O_RDONLY || flags & O_ACCMODE == O_RDWR,
                opened_as_write: flags & O_ACCMODE == O_WRONLY || flags & O_ACCMODE == O_RDWR,
                current_perm,
            }
        } else {
            self.open_existing(path.as_bytes(), flags, uid, gid)?
        };

        let fd = self.next_fd;
        self.next_fd += 1;

        self.handles.insert(fd, handle);

        Ok(fd)
    }
    fn rmdir(&mut self, path: &str, uid: u32, gid: u32) -> Result<usize> {
        self.remove_dentry(path.as_bytes(), uid, gid, true)
    }
    fn unlink(&mut self, path: &str, uid: u32, gid: u32) -> Result<usize> {
        self.remove_dentry(path.as_bytes(), uid, gid, false)
    }
    fn dup(&mut self, old_fd: usize, _buf: &[u8]) -> Result<usize> {
        let handle = self
            .handles
            .get_mut(&old_fd)
            .ok_or(Error::new(EBADF))?
            .clone();

        let fd = self.next_fd;
        self.next_fd += 1;

        self.handles.insert(fd, handle);
        Ok(fd)
    }
    fn read(&mut self, fd: usize, buf: &mut [u8]) -> Result<usize> {
        let handle = self.handles.get_mut(&fd).ok_or(Error::new(EBADF))?;
        let file = self
            .filesystem
            .files
            .get_mut(&handle.inode)
            .ok_or(Error::new(EBADFD))?;

        if !handle.opened_as_read {
            return Err(Error::new(EBADF));
        }

        match file.data {
            FileData::File(ref bytes) => {
                if file.mode & MODE_TYPE == MODE_DIR {
                    return Err(Error::new(EBADFD));
                }

                if handle.offset >= bytes.len() {
                    return Ok(0);
                }
                let bytes_to_read =
                    cmp::min(bytes.len(), buf.len() + handle.offset) - handle.offset;
                buf[..bytes_to_read]
                    .copy_from_slice(&bytes[handle.offset..handle.offset + bytes_to_read]);
                handle.offset += bytes_to_read;
                Ok(bytes_to_read)
            }
            FileData::Directory(ref entries) => {
                if file.mode & MODE_TYPE != MODE_DIR {
                    return Err(Error::new(EBADFD));
                }
                // directories require the execute permission to be listed
                if handle.current_perm & 0o1 == 0 {
                    return Err(Error::new(EBADF));
                }

                let mut bytes_to_skip = handle.offset;
                let mut bytes_left_to_read = buf.len();
                let mut bytes_read = 0;

                for DirEntry {
                    name: entry_bytes, ..
                } in entries
                {
                    // skip the whole entry if it fits
                    if bytes_to_skip >= entry_bytes.len() {
                        bytes_to_skip -= entry_bytes.len();
                        continue;
                    }

                    let bytes_to_read =
                        cmp::min(entry_bytes.len() + 1 - bytes_to_skip, bytes_left_to_read);

                    let entry_bytes =
                        &entry_bytes[bytes_to_skip..bytes_to_skip + bytes_to_read - 1];
                    bytes_to_skip -= bytes_to_skip;

                    buf[bytes_read..bytes_read + bytes_to_read - 1]
                        .copy_from_slice(&entry_bytes[..bytes_to_read - 1]);
                    buf[bytes_read + bytes_to_read - 1] = b'\n';
                    bytes_left_to_read -= bytes_to_read;
                    bytes_read += bytes_to_read;
                    handle.offset += bytes_read;
                }
                Ok(bytes_read)
            }
        }
    }
    fn write(&mut self, fd: usize, buf: &[u8]) -> Result<usize> {
        let handle = self.handles.get_mut(&fd).ok_or(Error::new(EBADF))?;
        let file = self
            .filesystem
            .files
            .get_mut(&handle.inode)
            .ok_or(Error::new(EBADFD))?;

        if let &mut FileData::File(ref mut bytes) = &mut file.data {
            if file.mode & MODE_TYPE == MODE_DIR {
                return Err(Error::new(EBADFD));
            }

            // if there's a seek hole, fill it with 0 and continue writing.
            let end_off = handle.offset.checked_add(buf.len()).ok_or(Error::new(EOVERFLOW))?;
            if end_off > bytes.len() {
                let additional = end_off - bytes.len();
                bytes.try_reserve(additional).or(Err(Error::new(ENOMEM)))?;
                bytes.resize(end_off, 0u8);
            }
            bytes[handle.offset..][..buf.len()].copy_from_slice(buf);
            handle.offset = end_off;

            Ok(buf.len())
        } else {
            Err(Error::new(EISDIR))
        }
    }
    fn seek(&mut self, fd: usize, pos: isize, whence: usize) -> Result<isize> {
        let handle = self.handles.get_mut(&fd).ok_or(Error::new(EBADF))?;
        let file = self
            .filesystem
            .files
            .get_mut(&handle.inode)
            .ok_or(Error::new(EBADFD))?;

        handle.offset = match whence {
            SEEK_SET => cmp::max(0, pos),
            SEEK_CUR => cmp::max(
                0,
                pos + isize::try_from(handle.offset).or(Err(Error::new(EOVERFLOW)))?,
            ),
            SEEK_END => cmp::max(
                0,
                pos + isize::try_from(file.data.size()).or(Err(Error::new(EOVERFLOW)))?,
            ),
            _ => return Err(Error::new(EINVAL)),
        } as usize;
        Ok(handle.offset as isize)
    }
    fn fchmod(&mut self, fd: usize, mode: u16) -> Result<usize> {
        let handle = self.handles.get_mut(&fd).ok_or(Error::new(EBADF))?;
        let file = self
            .filesystem
            .files
            .get_mut(&handle.inode)
            .ok_or(Error::new(EBADFD))?;

        let cur_type = file.mode & MODE_TYPE;

        /*
        if mode & MODE_TYPE != 0 {
            return Err(Error::new(EINVAL));
        }
        */

        file.mode = mode | cur_type;

        Ok(0)
    }
    fn fchown(&mut self, fd: usize, uid: u32, gid: u32) -> Result<usize> {
        let handle = self.handles.get_mut(&fd).ok_or(Error::new(EBADF))?;
        let file = self
            .filesystem
            .files
            .get_mut(&handle.inode)
            .ok_or(Error::new(EBADFD))?;

        file.uid = uid;
        file.gid = gid;

        Ok(0)
    }
    fn fcntl(&mut self, fd: usize, _cmd: usize, _arg: usize) -> Result<usize> {
        if !self.handles.contains_key(&fd) {
            return Err(Error::new(EBADF));
        }
        Ok(0)
    }
    fn fevent(&mut self, fd: usize, _flags: EventFlags) -> Result<EventFlags> {
        if !self.handles.contains_key(&fd) {
            return Err(Error::new(EBADF));
        }
        Err(Error::new(ENOSYS))
    }
    fn mmap_prep(&mut self, fd: usize, _offset: u64, _size: usize, _flags: syscall::MapFlags) -> Result<usize> {
        if !self.handles.contains_key(&fd) {
            return Err(Error::new(EBADF));
        }
        // TODO
        Err(Error::new(ENOSYS))
    }
    fn fpath(&mut self, fd: usize, _buf: &mut [u8]) -> Result<usize> {
        if !self.handles.contains_key(&fd) {
            return Err(Error::new(EBADF));
        }
        // TODO
        Err(Error::new(ENOSYS))
    }
    fn frename(&mut self, fd: usize, _path: &str, _uid: u32, _gid: u32) -> Result<usize> {
        if !self.handles.contains_key(&fd) {
            return Err(Error::new(EBADF));
        }
        // TODO
        Err(Error::new(ENOSYS))
    }
    fn fstat(&mut self, fd: usize, stat: &mut Stat) -> Result<usize> {
        let handle = self.handles.get_mut(&fd).ok_or(Error::new(EBADF))?;
        let block_size = self.filesystem.block_size();
        let file = self
            .filesystem
            .files
            .get_mut(&handle.inode)
            .ok_or(Error::new(EBADFD))?;

        let size = file.data.size().try_into().or(Err(Error::new(EOVERFLOW)))?;

        *stat = Stat {
            st_mode: file.mode,
            st_uid: file.uid,
            st_gid: file.gid,
            st_ino: handle.inode.try_into().or(Err(Error::new(EOVERFLOW)))?,
            st_nlink: file.nlink.try_into().or(Err(Error::new(EOVERFLOW)))?,
            st_dev: 0,

            st_size: size,
            st_blksize: block_size,
            st_blocks: div_round_up(size, u64::from(block_size)),

            st_atime: file
                .atime
                .tv_sec
                .try_into()
                .or(Err(Error::new(EOVERFLOW)))?,
            st_atime_nsec: file
                .atime
                .tv_nsec
                .try_into()
                .or(Err(Error::new(EOVERFLOW)))?,

            st_ctime: file
                .ctime
                .tv_sec
                .try_into()
                .or(Err(Error::new(EOVERFLOW)))?,
            st_ctime_nsec: file
                .ctime
                .tv_nsec
                .try_into()
                .or(Err(Error::new(EOVERFLOW)))?,

            st_mtime: file
                .mtime
                .tv_sec
                .try_into()
                .or(Err(Error::new(EOVERFLOW)))?,
            st_mtime_nsec: file
                .mtime
                .tv_nsec
                .try_into()
                .or(Err(Error::new(EOVERFLOW)))?,
        };

        Ok(0)
    }
    fn fstatvfs(&mut self, fd: usize, stat: &mut StatVfs) -> Result<usize> {
        if !self.handles.contains_key(&fd) {
            return Err(Error::new(EBADF));
        }
        let abi_stat = libredox::call::fstatvfs(self.filesystem.memory_file.as_raw_fd() as usize)?;
        // TODO: From impl
        *stat = StatVfs {
            f_bavail: abi_stat.f_bavail as u64,
            f_bfree: abi_stat.f_bfree as u64,
            f_blocks: abi_stat.f_blocks as u64,
            f_bsize: abi_stat.f_bsize as u32,
        };

        Ok(0)
    }
    fn fsync(&mut self, fd: usize) -> Result<usize> {
        if !self.handles.contains_key(&fd) {
            return Err(Error::new(EBADF));
        }
        Ok(0)
    }
    fn ftruncate(&mut self, fd: usize, size: usize) -> Result<usize> {
        let handle = self.handles.get_mut(&fd).ok_or(Error::new(EBADF))?;
        let file = self
            .filesystem
            .files
            .get_mut(&handle.inode)
            .ok_or(Error::new(EBADFD))?;

        if file.mode & MODE_TYPE == MODE_DIR {
            return Err(Error::new(EISDIR));
        }
        match &mut file.data {
            &mut FileData::File(ref mut bytes) => {
                if size > bytes.len() {
                    let additional = size - bytes.len();
                    bytes.try_reserve(additional).or(Err(Error::new(ENOMEM)))?;
                    bytes.resize(size, 0u8)
                } else {
                    bytes.resize(size, 0u8)
                }
            }
            &mut FileData::Directory(_) => return Err(Error::new(EBADFD)),
        }
        Ok(0)
    }
    fn futimens(&mut self, fd: usize, times: &[TimeSpec]) -> Result<usize> {
        let handle = self.handles.get_mut(&fd).ok_or(Error::new(EBADF))?;
        let file = self
            .filesystem
            .files
            .get_mut(&handle.inode)
            .ok_or(Error::new(EBADFD))?;

        let new_atime = *times.get(0).ok_or(Error::new(EINVAL))?;
        let new_mtime = *times.get(1).ok_or(Error::new(EINVAL))?;

        file.atime = new_atime;
        file.mtime = new_mtime;

        Ok(0)
    }
    fn close(&mut self, fd: usize) -> Result<usize> {
        let inode_num = self.handles.remove(&fd).ok_or(Error::new(EBADF))?.inode;
        let inode = self
            .filesystem
            .files
            .get_mut(&inode_num)
            .ok_or(Error::new(EIO))?;

        inode.open_handles -= 1;

        if inode.nlink == 0 && inode.open_handles == 0 {
            self.filesystem.files.remove(&inode_num);
        }
        Ok(0)
    }
}
fn div_round_up<T>(numer: T, denom: T) -> T
where
    T: Copy
        + ops::Add<T, Output = T>
        + ops::Sub<T, Output = T>
        + ops::Div<T, Output = T>
        + From<u8>,
{
    (numer + (denom - T::from(1u8))) / denom
}
pub fn current_perm(file: &crate::filesystem::File, uid: u32, gid: u32) -> u8 {
    let perm = file.mode & MODE_PERM;

    if uid == 0 {
        // root doesn't have to be checked
        0o7
    } else if uid == file.uid {
        ((perm & 0o700) >> 6) as u8
    } else if gid == file.gid {
        ((perm & 0o70) >> 3) as u8
    } else {
        (perm & 0o7) as u8
    }
}
fn check_permissions(flags: usize, single_mode: u8) -> Result<()> {
    if flags & O_ACCMODE == O_RDONLY && single_mode & 0o4 == 0 {
        return Err(Error::new(EACCES));
    } else if flags & O_ACCMODE == O_WRONLY && single_mode & 0o2 == 0 {
        return Err(Error::new(EACCES));
    } else if flags & O_ACCMODE == O_RDWR && single_mode & 0o6 != 0o6 {
        return Err(Error::new(EACCES));
    }
    Ok(())
}
