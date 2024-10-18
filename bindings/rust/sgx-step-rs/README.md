# Safe Rust bindings for libsgxstep

Work in progress. Only provides wrappers for most of `enclave.h` and part of `pt.h`.

## Usage

Add the following dependency to your `Cargo.toml`: `sgx-step = "0.1"`.

The directory of `libsgxstep` can be configured by setting the `LIBSGXSTEP` environment variable.

## Cargo features

By default, the `sgx-step` crate depends on the Intel SGX PSW. This is optional and can be disabled by disabling the `sgx` feature of this crate.
Most functionality of the `sgx-step` remains available with this feature disabled.

## Safety

The goal of this crate is to provide safe bindings for `libsgxstep`.

There are still some problems left that will need to be resolved:

- You are not prevented from creating multiple enclaves, but `libsgxstep` currently does not support this. This may lead to unexpected behavior, as calls to `enclave.h` functions do not pass a reference to a specific enclave (see [#3](https://github.com/jovanbulck/sgx-step/issues/3)).
- `PageTableEntry` is currently not tied to the lifetime of an enclave for usability reasons. However, accessing a `PageTableEntry` after the enclave is destroyed is undefined behavior.
- The validity of a `PageTableEntry` is not fully clear, it may be necessary to call `mlock` when holding a `PageTableEntry` during execution of the enclave.
