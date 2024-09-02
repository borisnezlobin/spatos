use std::iter::Peekable;
use thiserror::Error;

use crate::{
    expansion::pipelines::{Input, PipeItem, PipeType, Pipeline, RedirectFrom, Redirection},
    parser::lexers::arguments::{Field, Levels, LevelsError},
    shell::Job,
    types::*,
};

const ARG_DEFAULT_SIZE: usize = 10;

/// An error produced during pipeline parsing
#[derive(Debug, PartialEq, Eq, Hash, Clone, Error)]
pub enum PipelineParsingError {
    // redirections
    /// No file was provided after the redirection output
    #[error("expected file argument after redirection for output")]
    NoRedirection,
    /// Heredocs are deprecated and were used
    #[error("heredocs are not a part of Ion. Use redirection and/or cat instead")]
    HeredocsDeprecated,
    /// No string was given to the herestring
    #[error("expected string argument after '<<<'")]
    NoHereStringArg,
    /// No file was provided after the input redirection
    #[error("expected file argument after redirection for input")]
    NoRedirectionArg,

    // quotes
    /// Unterminated double quotes
    #[error("unterminated double quote")]
    UnterminatedDoubleQuote,
    /// Unterminated single quotes
    #[error("unterminated single quote")]
    UnterminatedSingleQuote,

    // paired
    /// Error with paired tokens (parens, brackets & braces)
    #[error("{0}")]
    Paired(#[source] LevelsError),
}

impl From<LevelsError> for PipelineParsingError {
    fn from(cause: LevelsError) -> Self { Self::Paired(cause) }
}

trait AddItem<'a> {
    fn add_item(
        &mut self,
        redirection: RedirectFrom,
        args: Args,
        outputs: Vec<Redirection>,
        inputs: Vec<Input>,
    );
}

impl<'a> AddItem<'a> for Pipeline<Job> {
    fn add_item(
        &mut self,
        redirection: RedirectFrom,
        args: Args,
        outputs: Vec<Redirection>,
        inputs: Vec<Input>,
    ) {
        if !args.is_empty() {
            self.items.push(PipeItem::new(Job::new(args, redirection), outputs, inputs));
        }
    }
}

/// Collect pipelines in the input
#[derive(Debug, Clone)]
pub struct Collector<'a> {
    data: &'a str,
}

impl<'a> Collector<'a> {
    /// Add a new argument that is re
    fn push_arg<I>(
        &self,
        args: &mut Args,
        bytes: &mut Peekable<I>,
    ) -> Result<(), PipelineParsingError>
    where
        I: Iterator<Item = (usize, u8)>,
    {
        if let Some(v) = self.arg(bytes)? {
            args.push(v.into());
        }
        Ok(())
    }

    /// Attempt to add a redirection
    fn push_redir_to_output<I>(
        &self,
        from: RedirectFrom,
        outputs: &mut Vec<Redirection>,
        bytes: &mut Peekable<I>,
    ) -> Result<(), PipelineParsingError>
    where
        I: Iterator<Item = (usize, u8)>,
    {
        let append = if let Some(&(_, b'>')) = bytes.peek() {
            bytes.next();
            true
        } else {
            false
        };
        self.arg(bytes)?
            .ok_or(PipelineParsingError::NoRedirection)
            .map(|file| outputs.push(Redirection { from, file: file.into(), append }))
    }

    fn parse(&self) -> Result<Pipeline<Job>, PipelineParsingError> {
        let mut bytes = self.data.bytes().enumerate().peekable();
        let mut args = Args::with_capacity(ARG_DEFAULT_SIZE);
        let mut pipeline = Pipeline::new();
        let mut outputs: Vec<Redirection> = Vec::new();
        let mut inputs: Vec<Input> = Vec::new();

        while let Some(&(i, b)) = bytes.peek() {
            // Determine what production rule we are using based on the first character
            match b {
                b'&' => {
                    // We have effectively consumed this byte
                    bytes.next();
                    match bytes.peek() {
                        Some(&(_, b'>')) => {
                            // And this byte
                            bytes.next();
                            self.push_redir_to_output(
                                RedirectFrom::Both,
                                &mut outputs,
                                &mut bytes,
                            )?;
                        }
                        Some(&(_, b'|')) => {
                            bytes.next();
                            pipeline.add_item(
                                RedirectFrom::Both,
                                std::mem::replace(&mut args, Args::with_capacity(ARG_DEFAULT_SIZE)),
                                std::mem::take(&mut outputs),
                                std::mem::take(&mut inputs),
                            );
                        }
                        Some(&(_, b'!')) => {
                            bytes.next();
                            pipeline.pipe = PipeType::Disown;
                            break;
                        }
                        Some(_) | None => {
                            pipeline.pipe = PipeType::Background;
                            break;
                        }
                    }
                }
                b'^' => {
                    // We do not immediately consume this byte as it could just be the start of
                    // a new argument
                    match self.peek(i + 1) {
                        Some(b'>') => {
                            bytes.next();
                            bytes.next();
                            self.push_redir_to_output(
                                RedirectFrom::Stderr,
                                &mut outputs,
                                &mut bytes,
                            )?;
                        }
                        Some(b'|') => {
                            bytes.next();
                            bytes.next();
                            pipeline.add_item(
                                RedirectFrom::Stderr,
                                std::mem::replace(&mut args, Args::with_capacity(ARG_DEFAULT_SIZE)),
                                std::mem::take(&mut outputs),
                                std::mem::take(&mut inputs),
                            );
                        }
                        Some(_) | None => self.push_arg(&mut args, &mut bytes)?,
                    }
                }
                b'|' => {
                    bytes.next();
                    pipeline.add_item(
                        RedirectFrom::Stdout,
                        std::mem::replace(&mut args, Args::with_capacity(ARG_DEFAULT_SIZE)),
                        std::mem::take(&mut outputs),
                        std::mem::take(&mut inputs),
                    );
                }
                b'>' => {
                    bytes.next();
                    self.push_redir_to_output(RedirectFrom::Stdout, &mut outputs, &mut bytes)?;
                }
                b'<' => {
                    bytes.next();
                    if Some(b'<') == self.peek(i + 1) {
                        if Some(b'<') == self.peek(i + 2) {
                            // If the next two characters are arrows, then interpret
                            // the next argument as a herestring
                            bytes.next();
                            bytes.next();
                            if let Some(cmd) = self.arg(&mut bytes)? {
                                inputs.push(Input::HereString(cmd.into()));
                            } else {
                                return Err(PipelineParsingError::NoHereStringArg);
                            }
                        } else {
                            return Err(PipelineParsingError::HeredocsDeprecated);
                        }
                    } else if let Some(file) = self.arg(&mut bytes)? {
                        // Otherwise interpret it as stdin redirection
                        inputs.push(Input::File(file.into()));
                    } else {
                        return Err(PipelineParsingError::NoRedirectionArg);
                    }
                }
                // Skip over whitespace between jobs
                b' ' | b'\t' => {
                    bytes.next();
                }
                // Assume that the next character starts an argument and parse that argument
                _ => self.push_arg(&mut args, &mut bytes)?,
            }
        }

        pipeline.add_item(RedirectFrom::None, args, outputs, inputs);
        Ok(pipeline)
    }

    fn arg<I>(&self, bytes: &mut Peekable<I>) -> Result<Option<&'a str>, PipelineParsingError>
    where
        I: Iterator<Item = (usize, u8)>,
    {
        // XXX: I don't think its the responsibility of the pipeline parser to do this
        // but I'm not sure of a better solution
        let mut levels = Levels::default();
        let mut start = None;
        let mut end = None;
        // Array increments * 2 + 1; brace * 2
        // Supports up to 31 nested arrays
        let mut array_brace_counter: u32 = 0;

        // Skip over any leading whitespace
        while let Some(&(_, b)) = bytes.peek() {
            match b {
                b' ' | b'\t' => {
                    bytes.next();
                }
                _ => break,
            }
        }

        while let Some(&(i, b)) = bytes.peek() {
            if start.is_none() {
                start = Some(i)
            }
            match b {
                b'(' => {
                    levels.up(Field::Proc);
                    bytes.next();
                }
                b')' => {
                    levels.down(Field::Proc)?;
                    bytes.next();
                }
                b'[' => {
                    levels.up(Field::Array);
                    array_brace_counter = array_brace_counter.wrapping_mul(2) + 1;
                    bytes.next();
                }
                b']' => {
                    levels.down(Field::Array)?;
                    if array_brace_counter % 2 == 1 {
                        array_brace_counter = (array_brace_counter - 1) / 2;
                        bytes.next();
                    } else {
                        break;
                    }
                }
                b'{' => {
                    levels.up(Field::Braces);
                    array_brace_counter = array_brace_counter.wrapping_mul(2);
                    bytes.next();
                }
                b'}' => {
                    if array_brace_counter % 2 == 0 {
                        levels.down(Field::Braces)?;
                        array_brace_counter /= 2;
                        bytes.next();
                    } else {
                        break;
                    }
                }
                // This is a tricky one: we only end the argment if `^` is followed by a
                // redirection character
                b'^' => {
                    if levels.are_rooted() {
                        if let Some(next_byte) = self.peek(i + 1) {
                            // If the next byte is for stderr to file or next process, end this
                            // argument
                            if next_byte == b'>' || next_byte == b'|' {
                                end = Some(i);
                                break;
                            }
                        }
                    }
                    // Reaching this block means that either there is no next byte, or the next
                    // byte is none of '>' or '|', indicating that this is not the beginning of
                    // a redirection for stderr
                    bytes.next();
                }
                // Evaluate a quoted string but do not return it
                // We pass in i, the index of a quote, but start a character later. This ensures
                // the production rules will produce strings with the quotes intact
                b'"' => {
                    bytes.next();
                    self.double_quoted(bytes, i)?;
                }
                b'\'' => {
                    bytes.next();
                    self.single_quoted(bytes, i)?;
                }
                // If we see a backslash, assume that it is leading up to an escaped character
                // and skip the next character
                b'\\' => {
                    bytes.next();
                    bytes.next();
                }
                // If we see a byte from the follow set, we've definitely reached the end of
                // the arguments
                b'&' | b'|' | b'<' | b'>' | b' ' | b'\t' if levels.are_rooted() => {
                    end = Some(i);
                    break;
                }
                // By default just pop the next byte: it will be part of the argument
                _ => {
                    bytes.next();
                }
            }
        }

        levels.check()?;

        match (start, end) {
            (Some(i), Some(j)) if i < j => Ok(Some(&self.data[i..j])),
            (Some(i), None) => Ok(Some(&self.data[i..])),
            _ => Ok(None),
        }
    }

    fn double_quoted<I>(
        &self,
        bytes: &mut Peekable<I>,
        start: usize,
    ) -> Result<&'a str, PipelineParsingError>
    where
        I: Iterator<Item = (usize, u8)>,
    {
        while let Some(&(i, b)) = bytes.peek() {
            match b {
                b'\\' => {
                    bytes.next();
                }
                // We return an inclusive range to keep the quote type intact
                b'"' => {
                    bytes.next();
                    return Ok(&self.data[start..=i]);
                }
                _ => (),
            }
            bytes.next();
        }
        Err(PipelineParsingError::UnterminatedDoubleQuote)
    }

    fn single_quoted<I>(
        &self,
        bytes: &mut Peekable<I>,
        start: usize,
    ) -> Result<&'a str, PipelineParsingError>
    where
        I: Iterator<Item = (usize, u8)>,
    {
        while let Some(&(i, b)) = bytes.peek() {
            // We return an inclusive range to keep the quote type intact
            if b == b'\'' {
                bytes.next();
                return Ok(&self.data[start..=i]);
            }
            bytes.next();
        }
        Err(PipelineParsingError::UnterminatedSingleQuote)
    }

    const fn peek(&self, index: usize) -> Option<u8> {
        if index < self.data.len() {
            Some(self.data.as_bytes()[index])
        } else {
            None
        }
    }

    /// Collect a pipeline on the given data
    pub fn run<'builtins>(data: &'a str) -> Result<Pipeline<Job>, PipelineParsingError> {
        Collector::new(data).parse()
    }

    const fn new(data: &'a str) -> Self { Self { data } }
}

#[cfg(test)]
mod tests {
    use crate::{
        parser::{
            pipelines::{Input, PipeItem, PipeType, Pipeline, RedirectFrom, Redirection},
            statement::parse,
        },
        shell::{flow_control::Statement, Job, Shell},
    };

    #[test]
    fn stderr_redirection() {
        if let Statement::Pipeline(pipeline) =
            parse("git rev-parse --abbrev-ref HEAD ^> /dev/null").unwrap()
        {
            assert_eq!("git", &pipeline.items[0].job.args[0]);
            assert_eq!("rev-parse", &pipeline.items[0].job.args[1]);
            assert_eq!("--abbrev-ref", &pipeline.items[0].job.args[2]);
            assert_eq!("HEAD", &pipeline.items[0].job.args[3]);

            let expected = vec![Redirection {
                from:   RedirectFrom::Stderr,
                file:   "/dev/null".into(),
                append: false,
            }];

            assert_eq!(expected, pipeline.items[0].outputs);
        } else {
            panic!();
        }
    }

    #[test]
    fn braces() {
        if let Statement::Pipeline(pipeline) = parse("echo {a b} {a {b c}}").unwrap() {
            let items = pipeline.items;
            assert_eq!("{a b}", &items[0].job.args[1]);
            assert_eq!("{a {b c}}", &items[0].job.args[2]);
        } else {
            panic!();
        }
    }

    #[test]
    fn methods() {
        if let Statement::Pipeline(pipeline) =
            parse("echo @split(var, ', ') $join(array, ',')").unwrap()
        {
            let items = pipeline.items;
            assert_eq!("echo", &items[0].job.args[0]);
            assert_eq!("@split(var, ', ')", &items[0].job.args[1]);
            assert_eq!("$join(array, ',')", &items[0].job.args[2]);
        } else {
            panic!();
        }
    }

    #[test]
    fn nested_process() {
        if let Statement::Pipeline(pipeline) = parse("echo $(echo one $(echo two) three)").unwrap()
        {
            let items = pipeline.items;
            assert_eq!("echo", &items[0].job.args[0]);
            assert_eq!("$(echo one $(echo two) three)", &items[0].job.args[1]);
        } else {
            panic!();
        }
    }

    #[test]
    fn nested_array_process() {
        if let Statement::Pipeline(pipeline) = parse("echo @(echo one @(echo two) three)").unwrap()
        {
            let items = pipeline.items;
            assert_eq!("echo", &items[0].job.args[0]);
            assert_eq!("@(echo one @(echo two) three)", &items[0].job.args[1]);
        } else {
            panic!();
        }
    }

    #[test]
    fn quoted_process() {
        if let Statement::Pipeline(pipeline) = parse("echo \"$(seq 1 10)\"").unwrap() {
            let items = pipeline.items;
            assert_eq!("echo", &items[0].job.args[0]);
            assert_eq!("\"$(seq 1 10)\"", &items[0].job.args[1]);
            assert_eq!(2, items[0].job.args.len());
        } else {
            panic!();
        }
    }

    #[test]
    fn process() {
        if let Statement::Pipeline(pipeline) = parse("echo $(seq 1 10 | head -1)").unwrap() {
            let items = pipeline.items;
            assert_eq!("echo", &items[0].job.args[0]);
            assert_eq!("$(seq 1 10 | head -1)", &items[0].job.args[1]);
            assert_eq!(2, items[0].job.args.len());
        } else {
            panic!();
        }
    }

    #[test]
    fn array_process() {
        if let Statement::Pipeline(pipeline) = parse("echo @(seq 1 10 | head -1)").unwrap() {
            let items = pipeline.items;
            assert_eq!("echo", &items[0].job.args[0]);
            assert_eq!("@(seq 1 10 | head -1)", &items[0].job.args[1]);
            assert_eq!(2, items[0].job.args.len());
        } else {
            panic!();
        }
    }

    #[test]
    fn single_job_no_args() {
        if let Statement::Pipeline(pipeline) = parse("cat").unwrap() {
            let items = pipeline.items;
            assert_eq!(1, items.len());
            assert_eq!("cat", &items[0].job.args[0]);
            assert_eq!(1, items[0].job.args.len());
        } else {
            panic!();
        }
    }

    #[test]
    fn single_job_with_single_character_arguments() {
        if let Statement::Pipeline(pipeline) = parse("echo a b c").unwrap() {
            let items = pipeline.items;
            assert_eq!(1, items.len());
            assert_eq!("echo", &items[0].job.args[0]);
            assert_eq!("a", &items[0].job.args[1]);
            assert_eq!("b", &items[0].job.args[2]);
            assert_eq!("c", &items[0].job.args[3]);
            assert_eq!(4, items[0].job.args.len());
        } else {
            panic!();
        }
    }

    #[test]
    fn job_with_args() {
        if let Statement::Pipeline(pipeline) = parse("ls -al dir").unwrap() {
            let items = pipeline.items;
            assert_eq!(1, items.len());
            assert_eq!("ls", &items[0].job.args[0]);
            assert_eq!("-al", &items[0].job.args[1]);
            assert_eq!("dir", &items[0].job.args[2]);
        } else {
            panic!();
        }
    }

    #[test]
    fn parse_empty_string() {
        if let Statement::Default = parse("").unwrap() {
            return;
        } else {
            panic!();
        }
    }

    #[test]
    fn multiple_white_space_between_words() {
        if let Statement::Pipeline(pipeline) = parse("ls \t -al\t\tdir").unwrap() {
            let items = pipeline.items;
            assert_eq!(1, items.len());
            assert_eq!("ls", &items[0].job.args[0]);
            assert_eq!("-al", &items[0].job.args[1]);
            assert_eq!("dir", &items[0].job.args[2]);
        } else {
            panic!();
        }
    }

    #[test]
    fn trailing_whitespace() {
        if let Statement::Pipeline(pipeline) = parse("ls -al\t ").unwrap() {
            assert_eq!(1, pipeline.items.len());
            assert_eq!("ls", &pipeline.items[0].job.args[0]);
            assert_eq!("-al", &pipeline.items[0].job.args[1]);
        } else {
            panic!();
        }
    }

    #[test]
    fn double_quoting() {
        if let Statement::Pipeline(pipeline) = parse("echo \"a > 10\" \"a < 10\"").unwrap() {
            let items = pipeline.items;
            assert_eq!("\"a > 10\"", &items[0].job.args[1]);
            assert_eq!("\"a < 10\"", &items[0].job.args[2]);
            assert_eq!(3, items[0].job.args.len());
        } else {
            panic!()
        }
    }

    #[test]
    fn double_quoting_contains_single() {
        if let Statement::Pipeline(pipeline) = parse("echo \"Hello 'Rusty' World\"").unwrap() {
            let items = pipeline.items;
            assert_eq!(2, items[0].job.args.len());
            assert_eq!("\"Hello \'Rusty\' World\"", &items[0].job.args[1]);
        } else {
            panic!()
        }
    }

    #[test]
    fn multi_quotes() {
        if let Statement::Pipeline(pipeline) = parse("echo \"Hello \"Rusty\" World\"").unwrap() {
            let items = pipeline.items;
            assert_eq!(2, items[0].job.args.len());
            assert_eq!("\"Hello \"Rusty\" World\"", &items[0].job.args[1]);
        } else {
            panic!()
        }

        if let Statement::Pipeline(pipeline) = parse("echo \'Hello \'Rusty\' World\'").unwrap() {
            let items = pipeline.items;
            assert_eq!(2, items[0].job.args.len());
            assert_eq!("\'Hello \'Rusty\' World\'", &items[0].job.args[1]);
        } else {
            panic!()
        }
    }

    #[test]
    fn all_whitespace() {
        if let Statement::Default = parse("  \t ").unwrap() {
            return;
        } else {
            panic!();
        }
    }

    #[test]
    fn not_background_job() {
        if let Statement::Pipeline(pipeline) = parse("echo hello world").unwrap() {
            let items = pipeline.items;
            assert_eq!(RedirectFrom::None, items[0].job.redirection);
        } else {
            panic!();
        }
    }

    #[test]
    fn background_job() {
        if let Statement::Pipeline(pipeline) = parse("echo hello world&").unwrap() {
            assert_eq!(PipeType::Background, pipeline.pipe);
        } else {
            panic!();
        }

        if let Statement::Pipeline(pipeline) = parse("echo hello world &").unwrap() {
            assert_eq!(PipeType::Background, pipeline.pipe);
        } else {
            panic!();
        }
    }

    #[test]
    fn disown_job() {
        if let Statement::Pipeline(pipeline) = parse("echo hello world&!").unwrap() {
            assert_eq!(PipeType::Disown, pipeline.pipe);
        } else {
            panic!();
        }
    }

    #[test]
    fn lone_comment() {
        if let Statement::Default = parse("# ; \t as!!+dfa").unwrap() {
            return;
        } else {
            panic!();
        }
    }

    #[test]
    fn leading_whitespace() {
        if let Statement::Pipeline(pipeline) = parse("    \techo").unwrap() {
            let items = pipeline.items;
            assert_eq!(1, items.len());
            assert_eq!("echo", &items[0].job.args[0]);
        } else {
            panic!();
        }
    }

    #[test]
    fn single_quoting() {
        if let Statement::Pipeline(pipeline) = parse("echo '#!!;\"\\'").unwrap() {
            let items = pipeline.items;
            assert_eq!("'#!!;\"\\'", &items[0].job.args[1]);
        } else {
            panic!();
        }
    }

    #[test]
    fn mixed_quoted_and_unquoted() {
        if let Statement::Pipeline(pipeline) =
            parse("echo 123 456 \"ABC 'DEF' GHI\" 789 one'  'two").unwrap()
        {
            let items = pipeline.items;
            assert_eq!("123", &items[0].job.args[1]);
            assert_eq!("456", &items[0].job.args[2]);
            assert_eq!("\"ABC 'DEF' GHI\"", &items[0].job.args[3]);
            assert_eq!("789", &items[0].job.args[4]);
            assert_eq!("one'  'two", &items[0].job.args[5]);
        } else {
            panic!();
        }
    }

    #[test]
    fn several_blank_lines() {
        if let Statement::Default = parse("\n\n\n").unwrap() {
            return;
        } else {
            panic!();
        }
    }

    #[test]
    // FIXME: May need updating after resolution of which part of the pipe
    // the input redirection shoud be associated with.
    fn pipeline_with_redirection() {
        let input = "cat | echo hello | cat < stuff > other";
        if let Statement::Pipeline(pipeline) = parse(input).unwrap() {
            assert_eq!(3, pipeline.items.len());
            assert_eq!("cat", &pipeline.items[0].job.args[0]);
            assert_eq!("echo", &pipeline.items[1].job.args[0]);
            assert_eq!("hello", &pipeline.items[1].job.args[1]);
            assert_eq!("cat", &pipeline.items[2].job.args[0]);
            assert_eq!(vec![Input::File("stuff".into())], pipeline.items[2].inputs);
            assert_eq!("other", &pipeline.items[2].outputs[0].file);
            assert!(!pipeline.items[2].outputs[0].append);
            assert_eq!(input.to_owned(), pipeline.expand(&mut Shell::new()).unwrap().to_string());
        } else {
            panic!();
        }
    }

    #[test]
    // FIXME: May need updating after resolution of which part of the pipe
    // the input redirection shoud be associated with.
    fn pipeline_with_redirection_append() {
        if let Statement::Pipeline(pipeline) =
            parse("cat | echo hello | cat < stuff >> other").unwrap()
        {
            assert_eq!(3, pipeline.items.len());
            assert_eq!(Input::File("stuff".into()), pipeline.items[2].inputs[0]);
            assert_eq!("other", &pipeline.items[2].outputs[0].file);
            assert!(pipeline.items[2].outputs[0].append);
        } else {
            panic!();
        }
    }

    #[test]
    // Ensures no regression for infinite loop when args() hits
    // '^' while not in the top level
    fn args_loop_terminates() {
        if let Statement::Pipeline(pipeline) = parse("$(^) '$(^)'").unwrap() {
            assert_eq!("$(^)", &pipeline.items[0].job.args[0]);
            assert_eq!("\'$(^)\'", &pipeline.items[0].job.args[1]);
        } else {
            panic!();
        }
    }

    #[test]
    // FIXME: May need updating after resolution of which part of the pipe
    // the input redirection shoud be associated with.
    fn multiple_redirect() {
        let input = "cat < file1 <<< \"herestring\" | tr 'x' 'y' ^>> err &> both > out";
        let expected = Pipeline {
            items: vec![
                PipeItem {
                    job:     Job::new(args!["cat"], RedirectFrom::Stdout),
                    inputs:  vec![
                        Input::File("file1".into()),
                        Input::HereString("\"herestring\"".into()),
                    ],
                    outputs: Vec::new(),
                },
                PipeItem {
                    job:     Job::new(args!["tr", "'x'", "'y'"], RedirectFrom::None),
                    inputs:  Vec::new(),
                    outputs: vec![
                        Redirection {
                            from:   RedirectFrom::Stderr,
                            file:   "err".into(),
                            append: true,
                        },
                        Redirection {
                            from:   RedirectFrom::Both,
                            file:   "both".into(),
                            append: false,
                        },
                        Redirection {
                            from:   RedirectFrom::Stdout,
                            file:   "out".into(),
                            append: false,
                        },
                    ],
                },
            ],
            pipe:  PipeType::Normal,
        };
        assert_eq!(parse(input).unwrap(), Statement::Pipeline(expected));
    }

    #[test]
    // FIXME: May need updating after resolution of which part of the pipe
    // the input redirection shoud be associated with.
    fn pipeline_with_redirection_append_stderr() {
        let input = "cat | echo hello | cat < stuff ^>> other";
        let expected = Pipeline {
            items: vec![
                PipeItem {
                    job:     Job::new(args!["cat"], RedirectFrom::Stdout),
                    inputs:  Vec::new(),
                    outputs: Vec::new(),
                },
                PipeItem {
                    job:     Job::new(args!["echo", "hello"], RedirectFrom::Stdout),
                    inputs:  Vec::new(),
                    outputs: Vec::new(),
                },
                PipeItem {
                    job:     Job::new(args!["cat"], RedirectFrom::None),
                    inputs:  vec![Input::File("stuff".into())],
                    outputs: vec![Redirection {
                        from:   RedirectFrom::Stderr,
                        file:   "other".into(),
                        append: true,
                    }],
                },
            ],
            pipe:  PipeType::Normal,
        };
        assert_eq!(parse(input).unwrap(), Statement::Pipeline(expected));
    }

    #[test]
    // FIXME: May need updating after resolution of which part of the pipe
    // the input redirection shoud be associated with.
    fn pipeline_with_redirection_append_both() {
        let input = "cat | echo hello | cat < stuff &>> other";
        let expected = Pipeline {
            items: vec![
                PipeItem {
                    job: Job::new(args!["cat"], RedirectFrom::Stdout),

                    inputs:  Vec::new(),
                    outputs: Vec::new(),
                },
                PipeItem {
                    job: Job::new(args!["echo", "hello"], RedirectFrom::Stdout),

                    inputs:  Vec::new(),
                    outputs: Vec::new(),
                },
                PipeItem {
                    job: Job::new(args!["cat"], RedirectFrom::None),

                    inputs:  vec![Input::File("stuff".into())],
                    outputs: vec![Redirection {
                        from:   RedirectFrom::Both,
                        file:   "other".into(),
                        append: true,
                    }],
                },
            ],
            pipe:  PipeType::Normal,
        };
        assert_eq!(parse(input).unwrap(), Statement::Pipeline(expected));
    }

    #[test]
    // FIXME: May need updating after resolution of which part of the pipe
    // the input redirection shoud be associated with.
    fn pipeline_with_redirection_reverse_order() {
        if let Statement::Pipeline(pipeline) =
            parse("cat | echo hello | cat > stuff < other").unwrap()
        {
            assert_eq!(3, pipeline.items.len());
            assert_eq!(vec![Input::File("other".into())], pipeline.items[2].inputs);
            assert_eq!("stuff", &pipeline.items[2].outputs[0].file);
        } else {
            panic!();
        }
    }

    #[test]
    fn var_meets_quote() {
        if let Statement::Pipeline(pipeline) = parse("echo $x '{()}' test").unwrap() {
            assert_eq!(1, pipeline.items.len());
            assert_eq!("echo", &pipeline.items[0].job.args[0]);
            assert_eq!("$x", &pipeline.items[0].job.args[1]);
            assert_eq!("'{()}'", &pipeline.items[0].job.args[2]);
            assert_eq!("test", &pipeline.items[0].job.args[3]);
        } else {
            panic!();
        }

        if let Statement::Pipeline(pipeline) = parse("echo $x'{()}' test").unwrap() {
            assert_eq!(1, pipeline.items.len());
            assert_eq!("echo", &pipeline.items[0].job.args[0]);
            assert_eq!("$x'{()}'", &pipeline.items[0].job.args[1]);
            assert_eq!("test", &pipeline.items[0].job.args[2]);
        } else {
            panic!();
        }
    }

    #[test]
    fn herestring() {
        let input = "math <<< $(cat math.txt)";
        let expected = Pipeline {
            items: vec![PipeItem {
                job: Job::new(args!["math"], RedirectFrom::None),

                inputs:  vec![Input::HereString("$(cat math.txt)".into())],
                outputs: vec![],
            }],
            pipe:  PipeType::Normal,
        };
        assert_eq!(Statement::Pipeline(expected), parse(input).unwrap());
    }

    #[test]
    // FIXME: May need updating after resolution of which part of the pipe
    // the input redirection shoud be associated with.
    fn piped_herestring() {
        let input = "cat | tr 'o' 'x' <<< $VAR > out.log";
        let expected = Pipeline {
            items: vec![
                PipeItem {
                    job: Job::new(args!["cat"], RedirectFrom::Stdout),

                    inputs:  Vec::new(),
                    outputs: Vec::new(),
                },
                PipeItem {
                    job: Job::new(args!["tr", "'o'", "'x'"], RedirectFrom::None),

                    inputs:  vec![Input::HereString("$VAR".into())],
                    outputs: vec![Redirection {
                        from:   RedirectFrom::Stdout,
                        file:   "out.log".into(),
                        append: false,
                    }],
                },
            ],
            pipe:  PipeType::Normal,
        };
        assert_eq!(Statement::Pipeline(expected), parse(input).unwrap());
    }

    #[test]
    fn awk_tests() {
        if let Statement::Pipeline(pipeline) =
            parse("awk -v x=$x '{ if (1) print $1 }' myfile").unwrap()
        {
            assert_eq!(1, pipeline.items.len());
            assert_eq!("awk", &pipeline.items[0].job.args[0]);
            assert_eq!("-v", &pipeline.items[0].job.args[1]);
            assert_eq!("x=$x", &pipeline.items[0].job.args[2]);
            assert_eq!("'{ if (1) print $1 }'", &pipeline.items[0].job.args[3]);
            assert_eq!("myfile", &pipeline.items[0].job.args[4]);
        } else {
            panic!();
        }
    }

    #[test]
    fn escaped_filenames() {
        let input = "echo zardoz >> foo\\'bar";
        let expected = Pipeline {
            items: vec![PipeItem {
                job: Job::new(args!["echo", "zardoz"], RedirectFrom::None),

                inputs:  Vec::new(),
                outputs: vec![Redirection {
                    from:   RedirectFrom::Stdout,
                    file:   "foo\\'bar".into(),
                    append: true,
                }],
            }],
            pipe:  PipeType::Normal,
        };
        assert_eq!(parse(input).unwrap(), Statement::Pipeline(expected));
    }

    fn assert_parse_error(s: &str) {
        assert!(super::Collector::new(s).parse().is_err());
    }

    #[test]
    fn arrays_braces_out_of_order() {
        assert_parse_error("echo {[}]");
        assert_parse_error("echo [{]}");
    }

    #[test]
    fn unmatched_right_brackets() {
        assert_parse_error("]");
        assert_parse_error("}");
        assert_parse_error(")");
    }
}
