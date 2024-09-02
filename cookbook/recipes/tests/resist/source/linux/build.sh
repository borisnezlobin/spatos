#!/usr/bin/env bash

set -e

cargo run --release -- linux/spec.toml posix/*.toml | tee linux/spec.md
