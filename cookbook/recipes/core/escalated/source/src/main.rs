// TODO: Move file descriptor number allocation to relibc, eliminating SYS_WRITE unsafety (for
// SKMSG_FOBTAINFD).
//
//#![forbid(unsafe_code)]
#![deny(unsafe_op_in_unsafe_fn)]

use std::collections::HashMap;
use std::fs::File;
use std::io::{prelude::*, ErrorKind};
use std::mem::MaybeUninit;

use syscall::data::Packet;
use syscall::error::*;
use syscall::flag::*;
use syscall::number::*;
use syscall::scheme::SchemeMut;

use redox_exec::FdGuard;

struct MemoryFd(FdGuard);
struct ExecFd(FdGuard);
struct ContextFd(FdGuard);

struct Scheme {
    next_fd: usize,
    handles: HashMap<usize, Handle>,
    memory: MemoryFd,
}
enum Handle {
    AwaitingContextFd {
        pid: usize,
    },
    AwaitingExecFd {
        ctx: ContextFd,
    },
    AwaitingPath {
        exec: ExecFd,
        ctx: ContextFd,
    },
    AwaitingArgs {
        exec: ExecFd,
        ctx: ContextFd,
        path: Box<str>,
    },
    AwaitingEnvs {
        exec: ExecFd,
        ctx: ContextFd,
        path: Box<str>,
        args: Box<[u8]>,
    },
    AwaitingCwd {
        exec: ExecFd,
        ctx: ContextFd,
        path: Box<str>,
        args: Box<[u8]>,
        envs: Box<[u8]>,
    },
    // TODO: Make cwd an fd too?
    Finished {
        exec: ExecFd,
        ctx: ContextFd,
        path: Box<str>,
        args: Box<[u8]>,
        envs: Box<[u8]>,
        cwd: Box<str>,
    },
    Placeholder,
}

impl SchemeMut for Scheme {
    fn open(&mut self, _path: &str, _flags: usize, _uid: u32, _gid: u32) -> Result<usize> {
        unreachable!("open is handled manually")
    }
    fn write(&mut self, id: usize, buf: &[u8]) -> Result<usize> {
        let handle = self.handles.get_mut(&id).ok_or(Error::new(EBADF))?;

        let validate_utf8 = |buf| std::str::from_utf8(buf).map_err(|_| Error::new(EINVAL));

        match std::mem::replace(handle, Handle::Placeholder) {
            old @ (Handle::AwaitingExecFd { .. } | Handle::AwaitingContextFd { .. }) => {
                *handle = old;
                return Err(Error::new(EINVAL));
            }
            Handle::AwaitingPath { exec, ctx } => {
                *handle = Handle::AwaitingArgs {
                    exec,
                    ctx,
                    path: validate_utf8(buf)?.into(),
                }
            }
            Handle::AwaitingArgs {
                exec,
                ctx,
                path,
            } => {
                *handle = Handle::AwaitingEnvs {
                    exec,
                    ctx,
                    path,
                    args: buf.into(),
                }
            }
            Handle::AwaitingEnvs {
                exec,
                ctx,
                path,
                args,
            } => {
                *handle = Handle::AwaitingCwd {
                    exec,
                    ctx,
                    path,
                    args,
                    envs: buf.into(),
                }
            }
            Handle::AwaitingCwd {
                exec,
                ctx,
                path,
                args,
                envs,
            } => {
                *handle = Handle::Finished {
                    exec,
                    ctx,
                    path,
                    args,
                    envs,
                    cwd: validate_utf8(buf)?.into(),
                }
            }
            Handle::Finished { .. } => return Ok(0),

            Handle::Placeholder => {
                eprintln!("escalated: found placeholder handle with ID {id}");
                return Err(Error::new(EBADFD));
            }
        }
        Ok(buf.len())
    }
    fn close(&mut self, id: usize) -> Result<usize> {
        if let Handle::Finished {
            exec,
            ctx,
            path,
            args,
            envs,
            cwd,
        } = self.handles.remove(&id).ok_or(Error::new(EBADF))?
        {
            execute(&self.memory, ctx, exec, &path, &args, &envs, &cwd)?;
        }
        Ok(0)
    }
}
impl Scheme {
    fn ext_open(&mut self, pid: usize, _b: usize, c: usize, _d: usize) -> Result<usize> {
        // Path must be empty
        if c > 0 {
            return Err(Error::new(ENOENT));
        }
        let fd = self.next_fd;
        self.next_fd = self.next_fd.checked_add(1).ok_or(Error::new(EMFILE))?;
        self.handles.insert(fd, Handle::AwaitingContextFd { pid });

        Ok(fd)
    }
    fn ext_sendfd(
        &mut self,
        socket: &File,
        packet_id: u64,
        id: usize,
        flags: SendFdFlags,
        _arg: u64,
    ) -> Result<usize> {
        let handle = self.handles.get_mut(&id).ok_or(Error::new(EBADF))?;
        match std::mem::replace(handle, Handle::Placeholder) {
            Handle::AwaitingContextFd { pid } => {
                // SAFETY: socket is a scheme socket
                let fd = unsafe { fobtainfd(socket, packet_id) };

                if !flags.contains(SendFdFlags::EXCLUSIVE) {
                    return Err(Error::new(EBUSY));
                }
                // FIXME: Improve the kernel proc: scheme (necessary for security), so that we can
                // ensure no other contexts can modify the context handle sent here, after this
                // file descriptor has been verified to now be exclusively owned by escalated.

                // TODO: Find a better way to read the PID than fpath
                let handle_pid = {
                    const LEN: usize = 256;

                    let mut path = [0_u8; LEN];
                    let len = syscall::fpath(*fd, &mut path)?;
                    if len > path.len() {
                        println!("escalated: too long fpath len ({len} > {LEN})");
                        return Err(Error::new(EIO));
                    }
                    assert!(path.starts_with(b"proc:"));
                    let path = &path[5..];
                    let slash = path.iter().position(|c| *c == b'/').expect("malformed kernel proc: fpath");
                    let pid_str = core::str::from_utf8(&path[..slash]).expect("non-UTF8 kernel proc: fpath");

                    pid_str.parse().expect("invalid kernel proc: fpath pid")
                };
                if pid != handle_pid {
                    return Err(Error::new(EACCES));
                }

                *handle = Handle::AwaitingExecFd {
                    ctx: ContextFd(fd),
                }
            }
            Handle::AwaitingExecFd { ctx } => {
                let fd = unsafe { fobtainfd(socket, packet_id) };
                *handle = Handle::AwaitingPath {
                    exec: ExecFd(fd),
                    ctx,
                }
            }
            old => {
                *handle = old;
                return Err(Error::new(EBADF));
            }
        }
        Ok(0)
    }
}

// SAFETY: The socket must be a scheme socket, even though non-kernel schemes most likely can't
// write to our memory, and even though this invariant may not be enforceable by Rust's safety
// model.
unsafe fn fobtainfd(mut socket: &File, packet_id: u64) -> FdGuard {
    let mut dst_fd = MaybeUninit::uninit();
    let _ = socket
        .write(&Packet {
            id: packet_id,
            pid: 0,
            uid: 0,
            gid: 0,
            a: Error::mux(Err(Error::new(ESKMSG))),
            b: SKMSG_FOBTAINFD,
            c: dst_fd.as_mut_ptr() as usize,
            d: FobtainFdFlags::empty().bits(),
        })
        .expect("failed to run SKMSG_FOBTAINFD");
    FdGuard::new(unsafe { dst_fd.assume_init() })
}

fn execute(
    MemoryFd(memory): &MemoryFd,
    ContextFd(context_file): ContextFd,
    ExecFd(exec_file): ExecFd,
    path: &str,
    args: &[u8],
    envs: &[u8],
    cwd: &str,
) -> Result<()> {
    let (setuid, setgid) = {
        let mut stat = syscall::Stat::default();
        syscall::fstat(*exec_file, &mut stat)?;

        // TODO: Check dev so that the scheme is privileged enough to promise that a
        // higher-privilege user has indeed allowed escalation via setuid/setgid. Currently the
        // kernel forbids schemes that are not run as root, however.
        let scheme_is_adequately_privileged = |_st_dev| true;

        if !scheme_is_adequately_privileged(stat.st_dev) {
            return Err(Error::new(EPERM));
        }

        (
            Some(stat.st_uid).filter(|_| stat.st_mode & (libc::S_ISUID as u16) != 0),
            Some(stat.st_gid).filter(|_| stat.st_mode & (libc::S_ISGID as u16) != 0),
        )
    };

    if setuid.is_none() && setgid.is_none() {
        // Allowing escalated to do fexec without escalation would just allow DoS!
        return Err(Error::new(EACCES));
    }

    let setuid = setuid.map(|new_uid| {
        Ok((new_uid, FdGuard::new(syscall::dup(*context_file, b"uid")?)))
    }).transpose()?;
    let setgid = setgid.map(|new_gid| {
        Ok((new_gid, FdGuard::new(syscall::dup(*context_file, b"gid")?)))
    }).transpose()?;

    let address_space_fd = redox_exec::fexec_impl(
        exec_file,
        context_file,
        memory,
        path.as_bytes(),
        args.rsplit(|c| *c == b'\0').filter(|a| !a.is_empty()),
        envs.rsplit(|c| *c == b'\0').filter(|a| !a.is_empty()),
        args.len() + envs.len() + cwd.len() + 1,
        &redox_exec::ExtraInfo {
            cwd: Some(cwd.as_bytes()),
        },
        None,
    )?;

    // Replace the address space *before* changing the UID and GID, to prevent (currently
    // impossible) race conditions that would allow the caller to run things as root before
    // becoming the setuid/setgid process.
    drop(address_space_fd);

    if let Some((new_uid, context_uid_fd)) = setuid {
        let _ = syscall::write(*context_uid_fd, new_uid.to_string().as_bytes()).map_err(|_| Error::new(EIO))?;
    }
    if let Some((new_gid, context_gid_fd)) = setgid {
        let _ = syscall::write(*context_gid_fd, new_gid.to_string().as_bytes()).map_err(|_| Error::new(EIO))?;
    }

    // TODO: SIGKILL the process if anything here fails after the process's memory has started to
    // be overwritten.

    Ok(())
}

fn main() {
    redox_daemon::Daemon::new(move |daemon| {
        let memory = MemoryFd(FdGuard::new(
            syscall::open("memory:", O_CLOEXEC).expect("failed to open `memory:`"),
        ));
        // TODO: Linux kernel audit-like logging?
        let mut socket = File::create(":escalate").expect("failed to open scheme socket");

        let mut packet = Packet::default();
        let mut scheme = Scheme {
            next_fd: 1,
            handles: HashMap::new(),
            memory,
        };
        syscall::setrens(0, 0).expect("failed to setrens");
        daemon
            .ready()
            .expect("failed to signal escalate scheme readiness");

        'outer: loop {
            loop {
                match socket.read(&mut packet) {
                    Ok(0) => break 'outer,
                    Ok(_) => break,
                    Err(err) if err.kind() == ErrorKind::Interrupted => continue,
                    Err(other) => panic!("escalate: scheme failed with error: {:?}", other),
                }
            }
            // FIXME: Improved scheme trait, which provides the pid, for every single invocation.
            if packet.a == SYS_OPEN {
                packet.a = Error::mux(scheme.ext_open(packet.pid, packet.b, packet.c, packet.d));
            } else if packet.a == SYS_SENDFD {
                packet.a = Error::mux(scheme.ext_sendfd(
                    &socket,
                    packet.id,
                    packet.b,
                    SendFdFlags::from_bits_truncate(packet.c),
                    u64::from(packet.uid) | (u64::from(packet.gid) << 32),
                ));
            } else {
                scheme.handle(&mut packet);
            }

            loop {
                match socket.write(&packet) {
                    Ok(0) => break 'outer,
                    Ok(_) => break,
                    Err(err) if err.kind() == ErrorKind::Interrupted => continue,
                    Err(other) => panic!("escalate: scheme failed with error: {:?}", other),
                }
            }
        }
        std::process::exit(0)
    })
    .expect("failed to start escalate daemon");
}
