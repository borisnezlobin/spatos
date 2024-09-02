use super::{IonError, Shell};
use crate::{
    builtins::BuiltinFunction,
    expansion::{self, pipelines::RedirectFrom, Expander},
    types, Value,
};
use std::{fmt, fs::File, str};

#[derive(Clone)]
/// A shell job
pub struct Job {
    /// Job Arguments
    pub args:        types::Args,
    /// Redirection of the job (stdout |, stderr ^|, both &|, None)
    pub redirection: RedirectFrom,
}

impl Job {
    /// Get the job command (its first arg)
    pub fn command(&self) -> &types::Str { &self.args[0] }

    /// Takes the current job's arguments and expands them, one argument at a
    /// time, returning a new `Job` with the expanded arguments.
    pub fn expand<'a>(&self, shell: &mut Shell<'a>) -> expansion::Result<RefinedJob<'a>, IonError> {
        let mut args = types::Args::new();
        for arg in &self.args {
            args.extend(expand_arg(arg, shell)?);
        }

        Ok(if let Some(Value::Function(_)) = shell.variables.get(&self.args[0]) {
            RefinedJob::function(self.args.clone(), self.redirection)
        } else if let Some(bt) = shell.builtins.get(&args[0]) {
            RefinedJob::builtin(bt, args, self.redirection)
        } else {
            RefinedJob::external(args, self.redirection)
        })
    }

    /// Create new shell job
    pub fn new(args: types::Args, redirection: RedirectFrom) -> Self { Self { args, redirection } }
}

impl PartialEq for Job {
    fn eq(&self, other: &Self) -> bool {
        self.args == other.args && self.redirection == other.redirection
    }
}

impl fmt::Debug for Job {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(
            f,
            "Job {{ command: {}, args: {:?}, redirection: {:?} }}",
            self.args[0], self.args, self.redirection
        )
    }
}

/// Expands a given argument and returns it as an `Args`.
fn expand_arg(arg: &str, shell: &mut Shell<'_>) -> expansion::Result<types::Args, IonError> {
    let res = shell.expand_string(arg)?;
    if res.is_empty() {
        Ok(args![""])
    } else {
        Ok(res)
    }
}

/// This represents a job that has been processed and expanded to be run
/// as part of some pipeline
pub struct RefinedJob<'a> {
    /// Standard in
    pub stdin:       Option<File>,
    /// Standard out
    pub stdout:      Option<File>,
    /// Standard error
    pub stderr:      Option<File>,
    /// Arguments for spawning command
    pub args:        types::Args,
    /// Variant (external, builtin, function, cat, tee)
    pub var:         Variant<'a>,
    /// Redirection (stdout |, stderr ^|, both &|, None)
    pub redirection: RedirectFrom,
}

pub enum Variant<'a> {
    /// An external program that is executed by this shell
    External,
    /// A procedure embedded into Ion
    Builtin { main: BuiltinFunction<'a> },
    /// Functions can act as commands too!
    Function,
    /// Represents redirection into stdin from more than one source
    Cat { sources: Vec<File> },
    Tee {
        /// 0 for stdout, 1 for stderr
        items: (Option<TeeItem>, Option<TeeItem>),
    },
}

#[derive(Debug, Default)]
pub struct TeeItem {
    /// Where to read from for this tee. Generally only necessary if we need to tee both
    /// stdout and stderr.
    pub source: Option<File>,
    pub sinks:  Vec<File>,
}

impl TeeItem {
    pub const fn new() -> Self { Self { sinks: Vec::new(), source: None } }

    pub fn add(&mut self, sink: File) { self.sinks.push(sink); }

    /// Writes out to all destinations of a Tee. Takes an extra `RedirectFrom` argument in
    /// order to
    /// handle piping. `RedirectFrom` paradoxically indicates where we are piping **to**. It
    /// should
    /// never be `RedirectFrom`::Both`
    pub fn write_to_all(&mut self, extra: Option<RedirectFrom>) -> ::std::io::Result<()> {
        use std::{
            io::{self, Read, Write},
            os::unix::io::*,
        };
        fn write_out<R>(source: &mut R, sinks: &mut [File]) -> io::Result<()>
        where
            R: Read,
        {
            let mut buf = [0; 4096];
            loop {
                // TODO: Figure out how to not block on this read
                let len = source.read(&mut buf)?;
                if len == 0 {
                    return Ok(());
                }
                for file in sinks.iter_mut() {
                    file.write_all(&buf[..len])?;
                }
            }
        }
        let stdout = io::stdout();
        let stderr = io::stderr();
        match extra {
            None => {}
            Some(RedirectFrom::Stdout) => unsafe {
                self.sinks.push(File::from_raw_fd(stdout.as_raw_fd()))
            },
            Some(RedirectFrom::Stderr) => unsafe {
                self.sinks.push(File::from_raw_fd(stderr.as_raw_fd()))
            },
            Some(RedirectFrom::Both) => {
                panic!("logic error! extra should never be RedirectFrom::Both")
            }
            Some(RedirectFrom::None) => panic!("logic error! No need to tee if no redirections"),
        };
        if let Some(ref mut file) = self.source {
            write_out(file, &mut self.sinks)
        } else {
            let stdin = io::stdin();
            let mut stdin = stdin.lock();
            write_out(&mut stdin, &mut self.sinks)
        }
    }
}

impl<'a> RefinedJob<'a> {
    /// What gets executed
    pub fn command(&self) -> &types::Str { &self.args[0] }

    /// What the arguments are
    pub const fn args(&self) -> &types::Args { &self.args }

    /// Redirect stderr to file
    pub fn stderr(&mut self, file: File) {
        if let Variant::Cat { .. } = self.var {
            return;
        }

        self.stderr = Some(file);
    }

    /// We must fork on a function and builtin
    pub const fn needs_forking(&self) -> bool {
        !matches!(self.var, Variant::Function | Variant::Builtin { .. })
    }

    /// Redirect stdout to file
    pub fn stdout(&mut self, file: File) { self.stdout = Some(file); }

    /// Redirect stdin to some file
    pub fn stdin(&mut self, file: File) { self.stdin = Some(file); }

    /// Apply tee to redirect out and err
    pub fn tee(
        tee_out: Option<TeeItem>,
        tee_err: Option<TeeItem>,
        redirection: RedirectFrom,
    ) -> Self {
        Self {
            stdin: None,
            stdout: None,
            stderr: None,
            args: types::Args::new(),
            var: Variant::Tee { items: (tee_out, tee_err) },
            redirection,
        }
    }

    /// Apply cat on source files
    pub fn cat(sources: Vec<File>, redirection: RedirectFrom) -> Self {
        Self {
            stdin: None,
            stdout: None,
            stderr: None,
            args: types::Args::new(),
            var: Variant::Cat { sources },
            redirection,
        }
    }

    /// Apply function
    pub const fn function(args: types::Args, redirection: RedirectFrom) -> Self {
        Self { stdin: None, stdout: None, stderr: None, args, var: Variant::Function, redirection }
    }

    /// Apply builtin
    pub fn builtin(
        main: BuiltinFunction<'a>,
        args: types::Args,
        redirection: RedirectFrom,
    ) -> Self {
        Self {
            stdin: None,
            stdout: None,
            stderr: None,
            args,
            var: Variant::Builtin { main },
            redirection,
        }
    }

    /// Apply external program executed by this shell
    pub const fn external(args: types::Args, redirection: RedirectFrom) -> Self {
        Self { stdin: None, stdout: None, stderr: None, args, var: Variant::External, redirection }
    }
}
