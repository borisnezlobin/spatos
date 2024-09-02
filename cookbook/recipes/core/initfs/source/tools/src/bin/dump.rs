use std::ffi::OsString;

use anyhow::{Context, Result};
use clap::{Arg, Command};

use redox_initfs::InitFs;

fn main() -> Result<()> {
    let matches = Command::new("redox-initfs-dump")
        .about("dump initfs metadata")
        .version(clap::crate_version!())
        .author(clap::crate_authors!())
        .arg(
            Arg::new("IMAGE")
                .required(true)
                .help("specify the image to dump"),
        )
        .get_matches();

    let source = matches
        .get_one::<OsString>("IMAGE")
        .expect("expected the required arg IMAGE to exist");

    let bytes = std::fs::read(source).context("failed to read image into memory")?;
    let initfs = InitFs::new(&bytes).context("failed to parse initfs header")?;

    dbg!(initfs.header());

    for inode in initfs.all_inodes() {
        print!("{:?}: ", inode);

        let inode_struct = match initfs.get_inode(inode) {
            Some(s) => s,
            None => {
                println!("failed to obtain.");
                continue;
            }
        };
        print!(
            "mode={:#0o}, uid={}, gid={}, kind=",
            inode_struct.mode(),
            inode_struct.uid(),
            inode_struct.gid()
        );

        use redox_initfs::InodeKind;

        match inode_struct.kind() {
            InodeKind::Unknown => println!("(unknown)"),
            InodeKind::Dir(dir) => {
                print!("dir{{");
                let ec = match dir.entry_count().ok() {
                    Some(c) => c,
                    None => {
                        println!("(failed to get entry count)}}");
                        continue;
                    }
                };
                println!("entries=[");

                for entry in 0..ec {
                    let entry = match dir.get_entry(entry).ok().flatten() {
                        Some(e) => e,
                        None => {
                            println!("\t(unknown),");
                            continue;
                        }
                    };
                    let name = match entry.name().ok() {
                        Some(name) => name,
                        None => {
                            println!("\t(unknown name),");
                            continue;
                        }
                    };
                    println!(
                        "\t`{}` => {:?},",
                        String::from_utf8_lossy(name),
                        entry.inode()
                    );
                }

                println!("]}}");
            }
            InodeKind::File(file) => {
                print!("file{{");

                match file.data().ok() {
                    Some(d) => {
                        use std::hash::Hasher;
                        let mut hasher = twox_hash::XxHash64::with_seed(0);
                        hasher.write(d);
                        print!("len={}, hash={:#0x}", d.len(), hasher.finish());
                    }
                    None => {
                        print!("(failed to get data)");
                    }
                }

                println!("}}");
            }
        }
    }

    Ok(())
}
