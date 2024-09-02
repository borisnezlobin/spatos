use resist::resist;
use std::{env, process};

fn main() {
    let args: Vec<_> = env::args().skip(1).collect();
    if args.is_empty() {
        eprintln!("resist: no specifications provided");
        process::exit(1);
    }

    for arg in args {
        match resist(&arg) {
            Ok(()) => (),
            Err(err) => {
                eprintln!("failed to build '{}': {}", arg, err);
                process::exit(1);
            }
        }
    }
}
