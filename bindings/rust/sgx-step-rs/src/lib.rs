pub mod memory;
pub mod page_table;

use std::{
    borrow::Borrow,
    error::Error,
    ffi::{c_void, CStr, CString, NulError},
    ops::{Deref, DerefMut},
    ptr::null_mut,
};

pub use sgx_step_sys;
use sgx_step_sys::{
    edbgrd, get_enclave_base, get_enclave_limit, get_enclave_size, get_enclave_ssa_gprsgx_adrs,
    get_symbol_offset, gprsgx_region_t, print_enclave_info, register_enclave_info, set_debug_optin,
    PAGE_SIZE_4KiB,
};

#[cfg(feature = "sgx")]
pub use sgx_urts_sys;
#[cfg(feature = "sgx")]
use sgx_urts_sys::{
    _status_t_SGX_SUCCESS, sgx_create_enclave, sgx_destroy_enclave, sgx_enclave_id_t,
    sgx_launch_token_t,
};

#[non_exhaustive]
#[derive(Debug, Clone, Copy)]
pub enum EnclaveId {
    SGX(u64),
}

#[allow(unreachable_patterns)]
impl EnclaveId {
    pub fn sgx_eid(&self) -> Option<u64> {
        match self {
            &EnclaveId::SGX(id) => Some(id),
            _ => None,
        }
    }
}

/// Handle to an SGX enclave
#[derive(Debug)]
pub struct EnclaveRef(EnclaveId);

impl EnclaveRef {
    /// Create an `EnclaveRef` from the given `EnclaveId`.
    ///
    /// Useful for interacting with existing enclaves that were not created using
    /// the `Enclave` struct.
    ///
    /// Also useful if you want to use `sgx-step-rs` without the `sgx` feature
    /// that dependes on the Intel SGX SDK.
    ///
    /// # Safety
    ///
    /// The given enclave id must be valid. The returned `EnclaveRef` is only
    /// valid as long as the reference enclave exists.
    pub unsafe fn from_raw(eid: EnclaveId) -> Self {
        unsafe { register_enclave_info() }
        Self(eid)
    }

    pub fn id(&self) -> EnclaveId {
        self.0
    }

    pub fn base(&self) -> *mut c_void {
        unsafe { get_enclave_base() as *mut c_void }
    }

    pub fn size(&self) -> *mut c_void {
        unsafe { get_enclave_size() as *mut c_void }
    }

    pub fn limit(&self) -> *mut c_void {
        unsafe { get_enclave_limit() as *mut c_void }
    }

    pub fn ssa_gprsgx(&self) -> *mut c_void {
        unsafe { get_enclave_ssa_gprsgx_adrs() as *mut c_void }
    }

    pub fn end(&self) -> *mut c_void {
        ((self.ssa_gprsgx() as usize & !0xfff) - PAGE_SIZE_4KiB as usize) as *mut c_void
    }

    pub fn gprsgx_region(&self) -> gprsgx_region_t {
        unsafe {
            let mut region: gprsgx_region_t = std::mem::zeroed();
            edbgrd(
                self.ssa_gprsgx(),
                &mut region as *mut _ as *mut c_void,
                std::mem::size_of::<gprsgx_region_t>(),
            );
            region
        }
    }

    pub fn dump(&self) {
        unsafe { print_enclave_info() }
    }

    pub fn dump_gprsgx_region(&self) {
        unsafe { sgx_step_sys::dump_gprsgx_region(&mut self.gprsgx_region() as *mut _) }
    }

    pub fn symbol_offset(&self, name: &str) -> Result<*mut c_void, NulError> {
        let c_str = CString::new(name).unwrap();
        Ok(unsafe { get_symbol_offset(c_str.as_ptr()) as *mut c_void })
    }

    pub fn symbol_address(&self, name: &str) -> Result<*mut c_void, NulError> {
        self.symbol_offset(name)
            .map(|o| ((self.base() as usize) + (o as usize)) as *mut c_void)
    }
}

/// Owned SGX enclave
#[derive(Debug)]
pub struct Enclave(EnclaveRef);

impl Enclave {
    /// Create a new SGX enclave and register the enclave in SGX-Step
    #[cfg(feature = "sgx")]
    pub fn new_sgx(enclave_so: &str, debug: bool) -> Result<Self, Box<dyn Error>> {
        use sgx_step_sys::register_symbols;

        let mut token: sgx_launch_token_t = [0; 1024];
        let mut updated = 0;
        let mut eid: sgx_enclave_id_t = 0;

        let enclave_so = CString::new(enclave_so)?;

        let result = unsafe {
            sgx_create_enclave(
                enclave_so.as_ptr(),
                if debug { 1 } else { 0 },
                &mut token,
                &mut updated,
                &mut eid,
                null_mut(),
            )
        };

        if result == _status_t_SGX_SUCCESS {
            if debug {
                unsafe { set_debug_optin() };
            }

            unsafe { register_enclave_info() };

            unsafe { register_symbols(enclave_so.as_ptr()) };

            Ok(Self(EnclaveRef(EnclaveId::SGX(eid))))
        } else {
            Err(format!("Failed to create enclave! error code: {result}").into())
        }
    }

    /// Convert this enclave to an unowned enclave reference
    pub fn to_raw(self) -> EnclaveRef {
        let inner = self.0 .0;
        std::mem::forget(self);
        EnclaveRef(inner)
    }
}

impl AsRef<EnclaveRef> for Enclave {
    fn as_ref(&self) -> &EnclaveRef {
        &self.0
    }
}

impl Deref for Enclave {
    type Target = EnclaveRef;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

impl DerefMut for Enclave {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.0
    }
}

impl Drop for Enclave {
    fn drop(&mut self) {
        match self.id() {
            EnclaveId::SGX(id) => unsafe {
                #[cfg(feature = "sgx")]
                sgx_destroy_enclave(id);
                #[cfg(not(feature = "sgx"))]
                unreachable!(
                    "Owned SGX enclave dropped, but not compiled with SGX support! This is a bug!"
                );
            },
        }
    }
}

impl Borrow<EnclaveRef> for Enclave {
    fn borrow(&self) -> &EnclaveRef {
        &self.0
    }
}
