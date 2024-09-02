use std::{env, process};

mod filesystem;
mod scheme;

use redox_scheme::{RequestKind, SignalBehavior};

use self::scheme::Scheme;

fn main() {
    let scheme_name = env::args().nth(1).expect("Usage:\n\tramfs SCHEME_NAME");

    redox_daemon::Daemon::new(move |daemon| {
        let socket = redox_scheme::Socket::create(&scheme_name).expect("ramfs: failed to create socket");

        let mut scheme = Scheme::new(scheme_name).expect("ramfs: failed to initialize scheme");

        libredox::call::setrens(0, 0).expect("ramfs: failed to enter null namespace");

        daemon.ready().expect("ramfs: failed to mark daemon as ready");

        loop {
            let Some(request) = socket.next_request(SignalBehavior::Restart).expect("ramfs: failed to get next scheme request") else {
                break;
            };
            match request.kind() {
                RequestKind::Call(call) => {
                    let response = call.handle_scheme_mut(&mut scheme);

                    socket.write_responses(&[response], SignalBehavior::Restart).expect("ramfs: failed to write next scheme response");
                }
                _ => (),
            }

        }

        process::exit(0);
    }).expect("ramfs: failed to create daemon");
}
