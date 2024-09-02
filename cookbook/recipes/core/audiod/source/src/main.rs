extern crate syscall;

use std::mem::MaybeUninit;
use std::ptr::addr_of_mut;
use std::{mem, process, slice, thread};
use std::sync::{Arc, Mutex};

use libredox::flag;
use libredox::{Fd, error::Result};
use redox_scheme::{Socket, SignalBehavior};

use redox_daemon::Daemon;

use self::scheme::AudioScheme;

mod scheme;

extern "C" fn sigusr_handler(_sig: usize) {}

fn thread(scheme: Arc<Mutex<AudioScheme>>, pid: usize, mut hw_file: Fd) -> Result<()> {
    // Enter null namespace
    libredox::call::setrens(0, 0)?;

    loop {
        let buffer = scheme.lock().unwrap().buffer();
        let buffer_u8 = unsafe {
            slice::from_raw_parts(
                buffer.as_ptr() as *const u8,
                mem::size_of_val(&buffer),
            )
        };

        // Wake up the scheme thread
        libredox::call::kill(pid, libredox::flag::SIGUSR1 as u32)?;

        hw_file.write(&buffer_u8)?;
    }
}

fn daemon(daemon: Daemon) -> Result<()> {
    // Handle signals from the hw thread

    let new_sigaction = unsafe {
        let mut sigaction = MaybeUninit::<libc::sigaction>::uninit();
        addr_of_mut!((*sigaction.as_mut_ptr()).sa_flags).write(0);
        libc::sigemptyset(addr_of_mut!((*sigaction.as_mut_ptr()).sa_mask));
        addr_of_mut!((*sigaction.as_mut_ptr()).sa_sigaction).write(sigusr_handler as usize);
        sigaction.assume_init()
    };
    libredox::call::sigaction(flag::SIGUSR1, Some(&new_sigaction), None)?;

    let pid = libredox::call::getpid()?;

    let hw_file = Fd::open("audiohw:", flag::O_WRONLY | flag::O_CLOEXEC, 0)?;

    let socket = Socket::create("audio")?;

    let scheme = Arc::new(Mutex::new(AudioScheme::new()));

    // Spawn a thread to mix and send audio data
    let scheme_thread = scheme.clone();
    let _thread = thread::spawn(move || thread(scheme_thread, pid, hw_file));

    // Enter the null namespace - done after thread is created so
    // memory: can be accessed for stack allocation
    libredox::call::setrens(0, 0)?;

    // The scheme is now ready to accept requests, notify the original process
    daemon.ready()?;

    let mut pending = Vec::new();

    loop  {
        match socket.next_request(SignalBehavior::Interrupt) {
            Ok(Some(request)) => match request.handle_scheme_block_mut(&mut *scheme.lock().unwrap()) {
                Ok(response) => {
                    socket.write_responses(&[response], SignalBehavior::Restart)?;
                }
                Err(request) => pending.push(request),
            },
            Ok(None) => {},
            Err(err) => match err.errno {
                libredox::errno::EINTR => {},
                _ => return Err(err),
            }
        }

        let mut i = 0;
        while i < pending.len() {
            let request = pending[i];
            match request.handle_scheme_block_mut(&mut *scheme.lock().unwrap()) {
                Ok(response) => {
                    pending.remove(i);
                    socket.write_responses(&[response], SignalBehavior::Restart)?;
                }
                Err(_) => {
                    i += 1
                }
            }
        }
    }
}

fn main() {
    if let Err(err) = Daemon::new(|x| {
        match daemon(x) {
            Ok(()) => {
                process::exit(0);
            },
            Err(err) => {
                eprintln!("audiod: {}", err);
                process::exit(1);
            }
        }
    }) {
        eprintln!("audiod: {}", err);
        process::exit(1);
    }
}
