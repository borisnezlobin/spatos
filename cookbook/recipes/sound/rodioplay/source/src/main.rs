extern crate rodio;

use std::{env, process};
use std::fs::File;
use std::io::BufReader;

fn main() {
    let mut count = 0;
    for arg in env::args().skip(1) {
        eprintln!("{}: {}", count, arg);

        let file = File::open(arg).unwrap();
        let source = rodio::Decoder::new(BufReader::new(file)).unwrap();

        let device = rodio::default_output_device().unwrap();
        let sink = rodio::Sink::new(&device);
        sink.append(source);
        sink.sleep_until_end();

        count += 1;
    }

    if count == 0 {
        eprintln!("rodioplay [file]");
        process::exit(1);
    }
}
