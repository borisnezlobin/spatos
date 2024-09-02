use serde::{Deserialize, Serialize};
use std::{
    error::Error,
};

use crate::test::Test;

#[derive(Debug, Deserialize, Serialize)]
pub struct Spec {
    pub name: String,
    pub desc: Option<String>,
    #[serde(with = "tuple_vec_map")]
    pub tests: Vec<(String, Test)>,
}

impl Spec {
    pub fn build(&self) -> Result<(), Box<dyn Error>> {
        println!("# {}\n", self.name);

        if let Some(desc) = &self.desc {
            println!("{}\n", desc);
        }

        for (test_name, test) in self.tests.iter() {
            println!("## {}\n", test_name);
            test.build()?;
        }

        Ok(())
    }
}
