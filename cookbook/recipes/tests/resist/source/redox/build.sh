#!/usr/bin/env bash

set -e

redoxer run --output redox/spec.md -- --release -- redox/spec.toml posix/*.toml
