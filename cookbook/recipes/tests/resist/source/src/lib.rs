use std::{
    error::Error,
    fs,
};

mod spec;
mod test;

pub fn resist(spec_path: &str) -> Result<(), Box<dyn Error>> {
    let spec_toml = fs::read_to_string(spec_path)?;
    let spec = toml::from_str::<spec::Spec>(&spec_toml)?;
    spec.build()
}
