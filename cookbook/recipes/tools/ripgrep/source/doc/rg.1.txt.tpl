rg(1)
=====

Name
----
rg - recursively search current directory for lines matching a pattern


Synopsis
--------
*rg* [_OPTIONS_] _PATTERN_ [_PATH_...]

*rg* [_OPTIONS_] *-e* _PATTERN_... [_PATH_...]

*rg* [_OPTIONS_] *-f* _PATTERNFILE_... [_PATH_...]

*rg* [_OPTIONS_] *--files* [_PATH_...]

*rg* [_OPTIONS_] *--type-list*

*command* | *rg* [_OPTIONS_] _PATTERN_

*rg* [_OPTIONS_] *--help*

*rg* [_OPTIONS_] *--version*


DESCRIPTION
-----------
ripgrep (rg) recursively searches your current directory for a regex pattern.
By default, ripgrep will respect your .gitignore and automatically skip hidden
files/directories and binary files.

ripgrep's default regex engine uses finite automata and guarantees linear
time searching. Because of this, features like backreferences and arbitrary
look-around are not supported. However, if ripgrep is built with PCRE2, then
the --pcre2 flag can be used to enable backreferences and look-around.

ripgrep supports configuration files. Set RIPGREP_CONFIG_PATH to a
configuration file. The file can specify one shell argument per line. Lines
starting with '#' are ignored. For more details, see the man page or the
README.


REGEX SYNTAX
------------
ripgrep uses Rust's regex engine by default, which documents its syntax:
https://docs.rs/regex/*/regex/#syntax

ripgrep uses byte-oriented regexes, which has some additional documentation:
https://docs.rs/regex/*/regex/bytes/index.html#syntax

To a first approximation, ripgrep uses Perl-like regexes without look-around or
backreferences. This makes them very similar to the "extended" (ERE) regular
expressions supported by `egrep`, but with a few additional features like
Unicode character classes.

If you're using ripgrep with the --pcre2 flag, then please consult
https://www.pcre.org or the PCRE2 man pages for documentation on the supported
syntax.


POSITIONAL ARGUMENTS
--------------------
_PATTERN_::
  A regular expression used for searching. To match a pattern beginning with a
  dash, use the -e/--regexp option.

_PATH_::
  A file or directory to search. Directories are searched recursively. Paths
  specified expicitly on the command line override glob and ignore rules.


OPTIONS
-------
{OPTIONS}


EXIT STATUS
-----------
If ripgrep finds a match, then the exit status of the program is 0. If no match
could be found, then the exit status is non-zero.


CONFIGURATION FILES
-------------------
ripgrep supports reading configuration files that change ripgrep's default
behavior. The format of the configuration file is an "rc" style and is very
simple. It is defined by two rules:

    1. Every line is a shell argument, after trimming ASCII whitespace.
    2. Lines starting with _#_ (optionally preceded by any amount of
       ASCII whitespace) are ignored.

ripgrep will look for a single configuration file if and only if the
_RIPGREP_CONFIG_PATH_ environment variable is set and is non-empty.
ripgrep will parse shell arguments from this file on startup and will
behave as if the arguments in this file were prepended to any explicit
arguments given to ripgrep on the command line.

For example, if your ripgreprc file contained a single line:

    --smart-case

then the following command

    RIPGREP_CONFIG_PATH=wherever/.ripgreprc rg foo

would behave identically to the following command

    rg --smart-case foo

another example is adding types

    --type-add
    web:*.{html,css,js}*

would behave identically to the following command

    rg --type-add 'web:*.{html,css,js}*' foo

same with using globs

    --glob=!git/*

or

    --glob
    !git/*

would behave identically to the following command

    rg --glob '!git/*' foo

ripgrep also provides a flag, *--no-config*, that when present will suppress
any and all support for configuration. This includes any future support
for auto-loading configuration files from pre-determined paths.

Conflicts between configuration files and explicit arguments are handled
exactly like conflicts in the same command line invocation. That is,
this command:

    RIPGREP_CONFIG_PATH=wherever/.ripgreprc rg foo --case-sensitive

is exactly equivalent to

    rg --smart-case foo --case-sensitive

in which case, the *--case-sensitive* flag would override the *--smart-case*
flag.


SHELL COMPLETION
----------------
Shell completion files are included in the release tarball for Bash, Fish, Zsh
and PowerShell.

For *bash*, move `rg.bash` to `$XDG_CONFIG_HOME/bash_completion`
or `/etc/bash_completion.d/`.

For *fish*, move `rg.fish` to `$HOME/.config/fish/completions`.

For *zsh*, move `_rg` to one of your `$fpath` directories.


CAVEATS
-------
ripgrep may abort unexpectedly when using default settings if it searches a
file that is simultaneously truncated. This behavior can be avoided by passing
the --no-mmap flag which will forcefully disable the use of memory maps in all
cases.


VERSION
-------
{VERSION}


HOMEPAGE
--------
https://github.com/BurntSushi/ripgrep

Please report bugs and feature requests in the issue tracker.


AUTHORS
-------
Andrew Gallant <jamslam@gmail.com>
