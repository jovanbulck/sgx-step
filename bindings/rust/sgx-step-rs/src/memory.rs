use std::{
    ffi::c_void,
    io::{self, Read, Write},
};

use sgx_step_sys::{edbgrd, edbgwr};

/// Wrapper around `edbgwr` and `edbgrd` that implements `std::io::Read` and `std::io::Write`
/// to read or write a given number of bytes from a fixed address.
#[derive(Debug)]
pub struct EnclaveMemory(usize);

impl EnclaveMemory {
    /// Create a new `EnclaveMemory` handle to read or write from the given address.
    pub fn new(address: usize) -> Self {
        Self(address)
    }
}

impl Write for EnclaveMemory {
    fn write(&mut self, buf: &[u8]) -> io::Result<usize> {
        let rv = unsafe {
            edbgwr(
                self.0 as *const c_void,
                buf.as_ptr() as *mut c_void,
                buf.len(),
            )
        };
        if rv == -1 {
            Err(io::Error::other(format!(
                "failed to write to enclave memory: {rv}"
            )))
        } else {
            Ok(buf.len())
        }
    }

    fn flush(&mut self) -> io::Result<()> {
        Ok(())
    }
}

impl Read for EnclaveMemory {
    fn read(&mut self, buf: &mut [u8]) -> io::Result<usize> {
        let rv = unsafe {
            edbgrd(
                self.0 as *const c_void,
                buf.as_ptr() as *mut c_void,
                buf.len(),
            )
        };
        if rv == -1 {
            Err(io::Error::other(format!(
                "failed to read from enclave memory: {rv}"
            )))
        } else {
            Ok(buf.len())
        }
    }
}
