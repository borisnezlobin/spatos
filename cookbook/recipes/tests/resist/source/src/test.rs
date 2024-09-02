use serde::{Deserialize, Serialize};
use std::{
    error::Error,
    fs,
    io::Read,
    process::{
        Command,
        Stdio,
    },
    str,
};
use tempfile::tempdir;

#[allow(non_camel_case_types)]
#[derive(Debug, Deserialize, Serialize)]
pub enum TestKind {
    c,
    python,
    shell,
}

impl Default for TestKind {
    fn default() -> Self {
        Self::shell
    }
}

#[derive(Debug, Deserialize, Serialize)]
pub struct Test {
    pub desc: Option<String>,
    #[serde(default)]
    pub kind: TestKind,
    pub source: String,
    pub output: String,
}

impl Test {
    pub fn build(&self) -> Result<(), Box<dyn Error>> {
        if let Some(desc) = &self.desc {
            println!("{}\n", desc)
        }
        println!("Kind: `{:?}`\n", self.kind);
        println!("Source:\n```{:?}\n{}\n```\n", self.kind, self.source);

        let mut output = Vec::new();
        let mut pass = true;
        {
            let dir = tempdir()?;

            let mut child = match self.kind {
                TestKind::c => {
                    fs::write(dir.path().join("src.c"), &self.source)?;
                    Command::new("sh")
                        .arg("-ec")
                        .arg("gcc src.c -o bin && ./bin")
                        .current_dir(dir.path())
                        .stdout(Stdio::piped())
                        .spawn()?
                },
                TestKind::python => {
                    Command::new("python")
                        .arg("-c")
                        .arg(&self.source)
                        .current_dir(dir.path())
                        .stdout(Stdio::piped())
                        .spawn()?
                },
                TestKind::shell => {
                    Command::new("sh")
                        .arg("-ec")
                        .arg(&self.source)
                        .current_dir(dir.path())
                        .stdout(Stdio::piped())
                        .spawn()?
                },
            };

            //TODO: timeout, async read
            let mut stdout = child.stdout.take().unwrap();
            loop {
                let mut buf = [0; 4096];
                let count = stdout.read(&mut buf)?;
                output.extend_from_slice(&buf[..count]);

                if let Some(status) = child.try_wait()? {
                    println!("Status: `{}`\n", status);
                    if ! status.success() {
                        println!("**Status indicates failure**\n");
                        pass = false;
                    }
                    break;
                }
            }
        }

        let output = str::from_utf8(&output)?;
        println!("Output:\n```\n{}\n```\n", output);
        if output != self.output {
            println!("Expected:\n```\n{}\n```\n", self.output);
            println!("**Output does not match expected**\n");
            pass = false;
        }

        if pass {
            println!("Result: **PASS**\n")
        } else {
            println!("Result: **FAIL**\n")
        }

        Ok(())
    }
}
