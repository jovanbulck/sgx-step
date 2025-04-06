# Raw Rust bindings for libsgxstep

## Usage

Add the following dependency to your `Cargo.toml`: `sgx-step-sys = "0.1"`.

The directory of `libsgxstep` can be configured by setting the `LIBSGXSTEP` environment variable.

## Cargo features

By default, the `build` feature is enabled, which causes `sgx-step-sys` to build and link `libsgxstep` into your Rust application.

This feature can be disabled, in which case you must link with `libsgxstep` manually in the build script of your application, e.g. using `cargo::rustc-link-search` and `cargo::rustc-link-lib`.
