use super::Status;
use crate as ion_shell;
use crate::{types, Shell};
use builtins_proc::builtin;
use std::{
    fs,
    os::unix::fs::{FileTypeExt, MetadataExt, PermissionsExt},
    path::Path,
    time::SystemTime,
};

const QUICK_GUIDE: &str = r#"Usage: test [EXPRESSION]
Try 'test --help' for more information."#;

#[builtin(
    desc = "perform tests on files and text",
    man = r#"
SYNOPSIS
    test [EXPRESSION]

DESCRIPTION
    Tests the expressions given and returns an exit status of 0 if true, else 1.

OPTIONS
    --help
        prints this help text

    -n STRING
        the length of STRING is nonzero

    STRING
        equivalent to -n STRING

    -z STRING
        the length of STRING is zero

    STRING = STRING
        the strings are equivalent

    STRING != STRING
        the strings are not equal

    INTEGER -eq INTEGER
        the integers are equal

    INTEGER -ge INTEGER
        the first INTEGER is greater than or equal to the second INTEGER

    INTEGER -gt INTEGER
        the first INTEGER is greater than the second INTEGER

    INTEGER -le INTEGER
        the first INTEGER is less than or equal to the second INTEGER

    INTEGER -lt INTEGER
        the first INTEGER is less than the second INTEGER

    INTEGER -ne INTEGER
        the first INTEGER is not equal to the second INTEGER

    FILE -ef FILE
        both files have the same device and inode numbers

    FILE -nt FILE
        the first FILE is newer than the second FILE

    FILE -ot FILE
        the first file is older than the second FILE

    -b FILE
        FILE exists and is a block device

    -c FILE
        FILE exists and is a character device

    -d FILE
        FILE exists and is a directory

    -e FILE
        FILE exists

    -f FILE
        FILE exists and is a regular file

    -h FILE
        FILE exists and is a symbolic link (same as -L)

    -L FILE
        FILE exists and is a symbolic link (same as -h)

    -r FILE
        FILE exists and read permission is granted

    -s FILE
        FILE exists and has a file size greater than zero

    -S FILE
        FILE exists and is a socket

    -w FILE
        FILE exists and write permission is granted

    -x FILE
        FILE exists and execute (or search) permission is granted

EXAMPLES
    Test if the file exists:
        test -e FILE && echo "The FILE exists" || echo "The FILE does not exist"

    Test if the file exists and is a regular file, and if so, write to it:
        test -f FILE && echo "Hello, FILE" >> FILE || echo "Cannot write to a directory"

    Test if 10 is greater than 5:
        test 10 -gt 5 && echo "10 is greater than 5" || echo "10 is not greater than 5"

    Test if the user is running a 64-bit OS (POSIX environment only):
        test $(getconf LONG_BIT) = 64 && echo "64-bit OS" || echo "32-bit OS"

AUTHOR
    Written by Michael Murphy."#
)]
pub fn test(args: &[types::Str], _: &mut Shell<'_>) -> Status {
    match evaluate_arguments(&args[1..]) {
        Ok(true) => Status::TRUE,
        Ok(false) => Status::FALSE,
        Err(why) => Status::error(why),
    }
}

fn evaluate_arguments(arguments: &[types::Str]) -> Result<bool, types::Str> {
    match arguments.first() {
        Some(s) if s.starts_with('-') && s[1..].starts_with(char::is_alphabetic) => {
            // Access the second character in the flag string: this will be type of the
            // flag. If no flag was given, return `SUCCESS`
            s.chars().nth(1).map_or(Ok(true), |flag| {
                // If no argument was given, return `SUCCESS`
                arguments.get(1).map_or(Ok(true), {
                    |arg|
                    // Match the correct function to the associated flag
                    Ok(match_flag_argument(flag, arg))
                })
            })
        }
        Some(arg) => {
            // If there is no operator, check if the first argument is non-zero
            arguments.get(1).map_or_else(
                || Ok(!arg.is_empty()),
                |operator| {
                    // If there is no right hand argument, a condition was expected
                    let right_arg = arguments
                        .get(2)
                        .ok_or_else(|| types::Str::from("parse error: condition expected"))?;
                    evaluate_expression(arg, operator, right_arg)
                },
            )
        }
        None => {
            println!("{}", QUICK_GUIDE);
            Ok(false)
        }
    }
}

fn evaluate_expression(first: &str, operator: &str, second: &str) -> Result<bool, types::Str> {
    match operator {
        "=" | "==" => Ok(first == second),
        "!=" => Ok(first != second),
        "-ef" => Ok(files_have_same_device_and_inode_numbers(first, second)),
        "-nt" => Ok(file_is_newer_than(first, second)),
        "-ot" => Ok(file_is_newer_than(second, first)),
        _ => {
            let (left, right) = parse_integers(first, second)?;
            match operator {
                "-eq" => Ok(left == right),
                "-ge" => Ok(left >= right),
                "-gt" => Ok(left > right),
                "-le" => Ok(left <= right),
                "-lt" => Ok(left < right),
                "-ne" => Ok(left != right),
                _ => Err(format!("test: unknown condition: {:?}", operator).into()),
            }
        }
    }
}

/// Exits SUCCESS if both files have the same device and inode numbers
fn files_have_same_device_and_inode_numbers(first: &str, second: &str) -> bool {
    // Obtain the device and inode of the first file or return FAILED
    get_dev_and_inode(first).map_or(false, |left| {
        // Obtain the device and inode of the second file or return FAILED
        get_dev_and_inode(second).map_or(false, |right| {
            // Compare the device and inodes of the
            // first and second files
            left == right
        })
    })
}

/// Obtains the device and inode numbers of the file specified
fn get_dev_and_inode(filename: &str) -> Option<(u64, u64)> {
    fs::metadata(filename).map(|file| (file.dev(), file.ino())).ok()
}

/// Exits SUCCESS if the first file is newer than the second file.
fn file_is_newer_than(first: &str, second: &str) -> bool {
    // Obtain the modified file time of the first file or return FAILED
    get_modified_file_time(first).map_or(false, |left| {
        // Obtain the modified file time of the second file or return
        // FAILED
        get_modified_file_time(second).map_or(false, |right| {
            // If the first file is newer
            // than the right file,
            // return SUCCESS
            left > right
        })
    })
}

/// Obtain the time the file was last modified as a `SystemTime` type.
fn get_modified_file_time(filename: &str) -> Option<SystemTime> {
    fs::metadata(filename).ok().and_then(|file| file.modified().ok())
}

/// Attempt to parse a &str as a usize.
fn parse_integers(left: &str, right: &str) -> Result<(Option<isize>, Option<isize>), types::Str> {
    let parse_integer = |input: &str| -> Result<Option<isize>, types::Str> {
        match input
            .parse::<isize>()
            .map_err(|_| format!("test: integer expression expected: {:?}", input))
        {
            Err(why) => Err(why.into()),
            Ok(res) => Ok(Some(res)),
        }
    };

    parse_integer(left).and_then(|left| match parse_integer(right) {
        Ok(right) => Ok((left, right)),
        Err(why) => Err(why),
    })
}

/// Matches flag arguments to their respective functionaity when the `-`
/// character is detected.
fn match_flag_argument(flag: char, argument: &str) -> bool {
    // TODO: Implement missing flags
    match flag {
        'b' => file_is_block_device(argument),
        'c' => file_is_character_device(argument),
        'd' => file_is_directory(argument),
        'e' => file_exists(argument),
        'f' => file_is_regular(argument),
        //'g' => file_is_set_group_id(argument),
        // 'G' => file_is_owned_by_effective_group_id(argument),
        'h' | 'L' => file_is_symlink(argument),
        //'k' => file_has_sticky_bit(argument),
        // 'O' => file_is_owned_by_effective_user_id(argument),
        // 'p' => file_is_named_pipe(argument),
        'r' => file_has_read_permission(argument),
        's' => file_size_is_greater_than_zero(argument),
        'S' => file_is_socket(argument),
        //'t' => file_descriptor_is_opened_on_a_terminal(argument),
        'w' => file_has_write_permission(argument),
        'x' => file_has_execute_permission(argument),
        'n' => !argument.is_empty(),
        'z' => argument.is_empty(),
        _ => true,
    }
}

/// Exits SUCCESS if the file size is greather than zero.
fn file_size_is_greater_than_zero(filepath: &str) -> bool {
    fs::metadata(filepath).ok().map_or(false, |metadata| metadata.len() > 0)
}

/// Exits SUCCESS if the file has read permissions. This function is rather low level because
/// Rust currently does not have a higher level abstraction for obtaining non-standard file modes.
/// To extract the permissions from the mode, the bitwise AND operator will be used and compared
/// with the respective read bits.
fn file_has_read_permission(filepath: &str) -> bool {
    const USER: u32 = 0b1_0000_0000;
    const GROUP: u32 = 0b10_0000;
    const GUEST: u32 = 0b100;

    // Collect the mode of permissions for the file
    fs::metadata(filepath)
        .map(|metadata| metadata.permissions().mode())
        .ok()
        // If the mode is equal to any of the above, return `SUCCESS`
        .map_or(false, |mode| mode & (USER + GROUP + GUEST) != 0)
}

/// Exits SUCCESS if the file has write permissions. This function is rather low level because
/// Rust currently does not have a higher level abstraction for obtaining non-standard file modes.
/// To extract the permissions from the mode, the bitwise AND operator will be used and compared
/// with the respective write bits.
fn file_has_write_permission(filepath: &str) -> bool {
    const USER: u32 = 0b1000_0000;
    const GROUP: u32 = 0b10000;
    const GUEST: u32 = 0b10;

    // Collect the mode of permissions for the file
    fs::metadata(filepath)
        .map(|metadata| metadata.permissions().mode())
        .ok()
        // If the mode is equal to any of the above, return `SUCCESS`
        .map_or(false, |mode| mode & (USER + GROUP + GUEST) != 0)
}

/// Exits SUCCESS if the file has execute permissions. This function is rather low level because
/// Rust currently does not have a higher level abstraction for obtaining non-standard file modes.
/// To extract the permissions from the mode, the bitwise AND operator will be used and compared
/// with the respective execute bits.
/// Note: This function is 1:1 the same as `src/builtins/exists.rs:file_has_execute_permission`
/// If you change the following function, please also update the one in src/builtins/exists.rs
fn file_has_execute_permission(filepath: &str) -> bool {
    const USER: u32 = 0b100_0000;
    const GROUP: u32 = 0b1000;
    const GUEST: u32 = 0b1;

    // Collect the mode of permissions for the file
    fs::metadata(filepath)
        .map(|metadata| metadata.permissions().mode())
        .ok()
        // If the mode is equal to any of the above, return `SUCCESS`
        .map_or(false, |mode| mode & (USER + GROUP + GUEST) != 0)
}

/// Exits SUCCESS if the file argument is a socket
fn file_is_socket(filepath: &str) -> bool {
    fs::metadata(filepath).ok().map_or(false, |metadata| metadata.file_type().is_socket())
}

/// Exits SUCCESS if the file argument is a block device
fn file_is_block_device(filepath: &str) -> bool {
    fs::metadata(filepath).ok().map_or(false, |metadata| metadata.file_type().is_block_device())
}

/// Exits SUCCESS if the file argument is a character device
fn file_is_character_device(filepath: &str) -> bool {
    fs::metadata(filepath).ok().map_or(false, |metadata| metadata.file_type().is_char_device())
}

/// Exits SUCCESS if the file exists
fn file_exists(filepath: &str) -> bool { Path::new(filepath).exists() }

/// Exits SUCCESS if the file is a regular file
fn file_is_regular(filepath: &str) -> bool {
    fs::metadata(filepath).ok().map_or(false, |metadata| metadata.file_type().is_file())
}

/// Exits SUCCESS if the file is a directory
fn file_is_directory(filepath: &str) -> bool {
    fs::metadata(filepath).ok().map_or(false, |metadata| metadata.file_type().is_dir())
}

/// Exits SUCCESS if the file is a symbolic link
fn file_is_symlink(filepath: &str) -> bool {
    fs::symlink_metadata(filepath).ok().map_or(false, |metadata| metadata.file_type().is_symlink())
}

#[test]
fn test_empty_str() {
    let eval = |args: Vec<types::Str>| evaluate_arguments(&args);
    assert_eq!(eval(vec!["".into()]), Ok(false));
    assert_eq!(eval(vec!["c".into(), "=".into(), "".into()]), Ok(false));
}

#[test]
fn test_integers_arguments() {
    fn vec_string(args: &[&str]) -> Vec<types::Str> { args.iter().map(|s| (*s).into()).collect() }
    // Equal To
    assert_eq!(evaluate_arguments(&vec_string(&["10", "-eq", "10"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["10", "-eq", "5"])), Ok(false));
    assert_eq!(evaluate_arguments(&vec_string(&["-10", "-eq", "-10"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["-10", "-eq", "10"])), Ok(false));

    // Greater Than or Equal To
    assert_eq!(evaluate_arguments(&vec_string(&["10", "-ge", "10"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["10", "-ge", "5"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["5", "-ge", "10"])), Ok(false));
    assert_eq!(evaluate_arguments(&vec_string(&["-9", "-ge", "-10"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["-10", "-ge", "-10"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["-10", "-ge", "10"])), Ok(false));

    // Less Than or Equal To
    assert_eq!(evaluate_arguments(&vec_string(&["5", "-le", "5"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["5", "-le", "10"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["10", "-le", "5"])), Ok(false));
    assert_eq!(evaluate_arguments(&vec_string(&["-11", "-le", "-10"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["-10", "-le", "-10"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["10", "-le", "-10"])), Ok(false));

    // Less Than
    assert_eq!(evaluate_arguments(&vec_string(&["5", "-lt", "10"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["10", "-lt", "5"])), Ok(false));
    assert_eq!(evaluate_arguments(&vec_string(&["-11", "-lt", "-10"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["10", "-lt", "-10"])), Ok(false));

    // Greater Than
    assert_eq!(evaluate_arguments(&vec_string(&["10", "-gt", "5"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["5", "-gt", "10"])), Ok(false));
    assert_eq!(evaluate_arguments(&vec_string(&["-9", "-gt", "-10"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["-10", "-gt", "10"])), Ok(false));

    // Not Equal To
    assert_eq!(evaluate_arguments(&vec_string(&["10", "-ne", "5"])), Ok(true));
    assert_eq!(evaluate_arguments(&vec_string(&["5", "-ne", "5"])), Ok(false));
    assert_eq!(evaluate_arguments(&vec_string(&["-10", "-ne", "-10"])), Ok(false));
    assert_eq!(evaluate_arguments(&vec_string(&["-10", "-ne", "10"])), Ok(true));
}

#[test]
fn test_file_exists() {
    assert_eq!(file_exists("testing/empty_file"), true);
    assert_eq!(file_exists("this-does-not-exist"), false);
}

#[test]
fn test_file_is_regular() {
    assert_eq!(file_is_regular("testing/empty_file"), true);
    assert_eq!(file_is_regular("testing"), false);
}

#[test]
fn test_file_is_directory() {
    assert_eq!(file_is_directory("testing"), true);
    assert_eq!(file_is_directory("testing/empty_file"), false);
}

#[test]
#[cfg(not(target_os = "redox"))]
fn test_file_is_symlink() {
    assert_eq!(file_is_symlink("testing/symlink"), true);
    assert_eq!(file_is_symlink("testing/empty_file"), false);
}

#[test]
fn test_file_has_execute_permission() {
    assert_eq!(file_has_execute_permission("testing/executable_file"), true);
    assert_eq!(file_has_execute_permission("testing/empty_file"), false);
}

#[test]
fn test_file_size_is_greater_than_zero() {
    assert_eq!(file_size_is_greater_than_zero("testing/file_with_text"), true);
    assert_eq!(file_size_is_greater_than_zero("testing/empty_file"), false);
}
