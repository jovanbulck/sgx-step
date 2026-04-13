use sgx_step_sys::{
    accessed, clear_rsvd, dirty, executable, execute_disable, free_map, mark_accessed, mark_clean,
    mark_dirty, mark_executable, mark_execute_disable, mark_non_executable, mark_non_writable,
    mark_not_accessed, mark_not_execute_disable, mark_not_present, mark_present, mark_rsvd,
    mark_supervisor, mark_user, mark_writable, present, pt_level_t_PAGE, pt_level_t_PGD,
    pt_level_t_PMD, pt_level_t_PTE, pt_level_t_PUD, remap_page_table_level, rsvd, user, writable,
};

use std::ffi::c_void;

/// Level in the page table
#[derive(Debug, Clone, Copy)]
pub enum PageTableLevel {
    PGD,
    PUD,
    PMD,
    PTE,
    PAGE,
}

impl Into<u32> for PageTableLevel {
    fn into(self) -> u32 {
        match self {
            PageTableLevel::PGD => pt_level_t_PGD,
            PageTableLevel::PUD => pt_level_t_PUD,
            PageTableLevel::PMD => pt_level_t_PMD,
            PageTableLevel::PTE => pt_level_t_PTE,
            PageTableLevel::PAGE => pt_level_t_PAGE,
        }
    }
}

/// Page table entry for the page at a given address
#[derive(Debug)]
pub struct PageTableEntry(*mut u64);

unsafe impl Sync for PageTableEntry {}
unsafe impl Send for PageTableEntry {}

impl PageTableEntry {
    /// Create a new `PageTableEntry` for the page at the given address
    pub fn new(address: usize) -> Option<Self> {
        let ptr =
            unsafe { remap_page_table_level(address as *mut c_void, pt_level_t_PTE) } as *mut u64;
        if ptr.is_null() {
            None
        } else {
            Some(Self(ptr))
        }
    }

    pub fn accessed(&self) -> bool {
        accessed(unsafe { *self.0 }) > 0
    }

    pub fn mark_accessed(&mut self) {
        unsafe { *self.0 = mark_accessed(*self.0) }
    }

    pub fn mark_not_accessed(&mut self) {
        unsafe { *self.0 = mark_not_accessed(*self.0) }
    }

    pub fn dirty(&self) -> bool {
        dirty(unsafe { *self.0 }) > 0
    }

    pub fn mark_dirty(&mut self) {
        unsafe { *self.0 = mark_dirty(*self.0) }
    }

    pub fn mark_clean(&mut self) {
        unsafe { *self.0 = mark_clean(*self.0) }
    }

    pub fn writable(&self) -> bool {
        writable(unsafe { *self.0 }) > 0
    }

    pub fn mark_non_writable(&mut self) {
        unsafe { *self.0 = mark_non_writable(*self.0) }
    }

    pub fn mark_writable(&mut self) {
        unsafe { *self.0 = mark_writable(*self.0) }
    }

    pub fn user(&self) -> bool {
        user(unsafe { *self.0 }) > 0
    }

    pub fn mark_user(&mut self) {
        unsafe { *self.0 = mark_user(*self.0) }
    }

    pub fn mark_supervisor(&mut self) {
        unsafe { *self.0 = mark_supervisor(*self.0) }
    }

    pub fn execute_disable(&self) -> bool {
        execute_disable(unsafe { *self.0 }) > 0
    }

    pub fn mark_not_execute_disable(&mut self) {
        unsafe { *self.0 = mark_not_execute_disable(*self.0) }
    }

    pub fn mark_execute_disable(&mut self) {
        unsafe { *self.0 = mark_execute_disable(*self.0) }
    }

    pub fn executable(&self) -> bool {
        executable(unsafe { *self.0 }) > 0
    }

    pub fn mark_non_executable(&mut self) {
        unsafe { *self.0 = mark_non_executable(*self.0) }
    }

    pub fn mark_executable(&mut self) {
        unsafe { *self.0 = mark_executable(*self.0) }
    }

    pub fn present(&self) -> bool {
        present(unsafe { *self.0 }) > 0
    }

    pub fn mark_not_present(&mut self) {
        unsafe { *self.0 = mark_not_present(*self.0) }
    }

    pub fn mark_present(&mut self) {
        unsafe { *self.0 = mark_present(*self.0) }
    }

    pub fn rsvd(&self) -> bool {
        rsvd(unsafe { *self.0 }) > 0
    }

    pub fn mark_rsvd(&mut self) {
        unsafe { *self.0 = mark_rsvd(*self.0) }
    }

    pub fn clear_rsvd(&mut self) {
        unsafe { *self.0 = clear_rsvd(*self.0) }
    }
}

impl Drop for PageTableEntry {
    fn drop(&mut self) {
        unsafe { free_map(self.0 as *mut c_void) };
    }
}
