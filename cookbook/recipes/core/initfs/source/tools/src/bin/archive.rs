use std::path::Path;

use anyhow::{Context, Result};
use clap::{Arg, Command};

#[path = "../archive_common.rs"]
mod archive_common;
use self::archive_common::{self as archive, Args, DEFAULT_MAX_SIZE};

fn main() -> Result<()> {
    let matches = Command::new("redox-initfs-ar")
        .about("create an initfs image from a directory")
        .version(clap::crate_version!())
        .author(clap::crate_authors!())
        .arg(
            Arg::new("MAX_SIZE")
                .long("max-size")
                .short('m')
                .required(false)
                .help("Set the upper limit for how large the image can become (default 64 MiB)."),
        )
        .arg(
            Arg::new("SOURCE")
                .required(true)
                .help("Specify the source directory to build the image from."),
        )
        .arg(
            Arg::new("BOOTSTRAP_CODE")
                .required(false)
                .help("Specify the bootstrap ELF file to include in the image.")
        )
        .arg(
            Arg::new("OUTPUT")
                .required(true)
                .long("output")
                .short('o')
                .help("Specify the path of the new image file."),
        )
        .get_matches();

    env_logger::init();

    let max_size = if let Some(max_size_str) = matches.get_one::<String>("MAX_SIZE") {
        max_size_str
            .parse::<u64>()
            .context("expected an integer for MAX_SIZE")?
    } else {
        DEFAULT_MAX_SIZE
    };

    let source = matches
        .get_one::<String>("SOURCE")
        .expect("expected the required arg SOURCE to exist");

    let bootstrap_code = matches.get_one::<String>("BOOTSTRAP_CODE");

    let destination = matches
        .get_one::<String>("OUTPUT")
        .expect("expected the required arg OUTPUT to exist");

    let args = Args {
        source: Path::new(source),
        bootstrap_code: bootstrap_code.map(|bootstrap_code| Path::new(bootstrap_code)),
        destination_path: Path::new(destination),
        max_size,
    };
    archive::archive(&args)
}
