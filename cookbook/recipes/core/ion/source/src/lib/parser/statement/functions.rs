use crate::parser::lexers::assignments::{KeyBuf, KeyIterator, TypeError};
use thiserror::Error;

#[derive(Debug, PartialEq, Eq, Hash, Clone, Error)]
pub enum FunctionParseError {
    #[error("repeated argument name: '{0}'")]
    RepeatedArgument(String),
    #[error("{0}")]
    TypeError(#[source] TypeError),
}

/// The arguments expression given to a function declaration goes into here, which will be
/// converted into a tuple consisting of a `KeyIterator` iterator, which will collect type
/// information, and an optional description of the function.
pub fn parse_function(arg: &str) -> (KeyIterator<'_>, Option<&str>) {
    let mut parts = arg.splitn(2, "--");
    let (args, description) = (parts.next().unwrap().trim(), parts.next().map(str::trim));
    (KeyIterator::new(args), description)
}

/// All type information will be collected from the `KeyIterator` and stored into a vector. If a
/// type or argument error is detected, then that error will be returned instead. This is required
/// because of lifetime restrictions on `KeyIterator`, which will not live for the remainder of the
/// declared function's lifetime.
pub fn collect_arguments(args: KeyIterator<'_>) -> Result<Vec<KeyBuf>, FunctionParseError> {
    let mut keybuf: Vec<KeyBuf> = Vec::new();
    for arg in args {
        match arg {
            Ok(key) => {
                let key: KeyBuf = key.into();
                if keybuf.iter().any(|k| k.name == key.name) {
                    return Err(FunctionParseError::RepeatedArgument(key.name));
                } else {
                    keybuf.push(key);
                }
            }
            Err(e) => return Err(FunctionParseError::TypeError(e)),
        }
    }
    Ok(keybuf)
}

#[cfg(test)]
mod tests {
    use crate::parser::{
        lexers::assignments::{KeyBuf, Primitive},
        statement::functions::{collect_arguments, parse_function, FunctionParseError},
    };

    #[test]
    fn function_parsing() {
        let (arg_iter, description) = parse_function("a:int b:bool c[] d -- description");
        let args = collect_arguments(arg_iter);
        assert_eq!(
            args,
            Ok(vec![
                KeyBuf { name: "a".into(), kind: Primitive::Integer },
                KeyBuf { name: "b".into(), kind: Primitive::Boolean },
                KeyBuf { name: "c".into(), kind: Primitive::Array(Box::new(Primitive::Str)) },
                KeyBuf { name: "d".into(), kind: Primitive::Str },
            ])
        );
        assert_eq!(description, Some("description"))
    }

    #[test]
    fn function_repeated_arg() {
        let (arg_iter, description) = parse_function("a:bool b a[] -- failed def");
        let args = collect_arguments(arg_iter);
        assert_eq!(args, Err(FunctionParseError::RepeatedArgument("a".into())));
        assert_eq!(description, Some("failed def"));
    }
}
