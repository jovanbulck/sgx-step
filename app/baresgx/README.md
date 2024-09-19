# Minimal baresgx test enclave

This directory contains a minimal test enclave hand written in assembly using
the [`bare-sgx`](https://github.com/jovanbulck/bare-sgx) runtime. This setup
allows you to load the enclave on a bare-metal Linux platform without needing
any bloated SDK installation.

The test enclave consists of a single code page with _exactly_ 100 instructions,
which can be precisely single-stepped by SGX-Step, making it an ideal resource
for testing and debugging.
