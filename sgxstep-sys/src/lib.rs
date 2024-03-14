#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]

use std::{arch::asm, ffi::c_void};

include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

#[inline]
pub fn enable_tf() {
    unsafe {
        asm!("pushf", "orl $0x100, (%rsp)", "popf", options(att_syntax));
    }
}

#[inline]
pub fn disable_tf() {
    unsafe {
        asm!(
            "pushf",
            "andl $0xfffffeff, (%rsp)",
            "popf",
            options(att_syntax)
        );
    }
}

// apic.h

pub fn apic_icr_vector(n: u32) -> u32 {
    n & 0xFF
}

pub unsafe fn apic_write(reg: u32, v: u32) {
    if apic_base.is_null() {
        apic_init();
    }

    let addr = (apic_base as usize + reg as usize) as *mut u32;

    asm!(
        "movl {1:e}, ({0})",
        in(reg) addr,
        in(reg) v,
        options(att_syntax)
    );
}

pub unsafe fn apic_read(reg: u32) -> u32 {
    if apic_base.is_null() {
        apic_init();
    }

    let addr = (apic_base as usize + reg as usize) as *const u32;

    unsafe { *addr }
}

#[inline]
pub unsafe fn apic_timer_irq(tsc: u32) {
    apic_write(APIC_TMICT, tsc)
}

#[inline]
pub unsafe fn apic_send_ipi_self(n: u32) {
    apic_write(
        APIC_ICR_LOW,
        apic_icr_vector(n) | APIC_ICR_DELIVERY_FIXED | APIC_ICR_LEVEL_ASSERT | APIC_ICR_DEST_SELF,
    )
}

// cache.h

#[inline(always)]
pub fn reload(address: *const u8) -> u64 {
    let mut time: u64;

    unsafe {
        asm!(
            "mfence",
            "lfence",
            "rdtsc",
            "lfence",
            "movl %eax, %esi",
            "movl ({x}), %eax",
            "lfence",
            "rdtsc",
            "subl %esi, %eax",
            x = in(reg) address,
            out("ax") time,
            out("si") _,
            out("dx") _,
            options(att_syntax)
        );
    }

    time
}

#[inline(always)]
pub fn flush(p: *const u8) {
    unsafe {
        asm!(
            "mfence",
            "clflush 0({})",
            "mfence",
            in(reg) p,
            out("ax") _,
            options(att_syntax)
        );
    }
}

#[inline(always)]
pub fn maccess(p: *const u8) {
    unsafe {
        asm!(
            "mov ({}), %rax",
            in(reg) p,
            out("ax") _,
            options(att_syntax)
        );
    }
}

// enclave.h

pub unsafe fn edbgrd_erip() -> u64 {
    edbgrd_ssa_gprsgx(SGX_GPRSGX_RIP_OFFSET as i32)
}

pub unsafe fn edbgrd(adrs: *const c_void, res: *mut c_void, len: usize) -> i32 {
    edbgrdwr(adrs as *mut c_void, res, len as i32, 0)
}
pub unsafe fn edbgwr(adrs: *const c_void, res: *mut c_void, len: usize) -> i32 {
    edbgrdwr(adrs as *mut c_void, res, len as i32, 1)
}

// gdt.h

pub fn desc_base(d: &desc_t) -> u32 {
    d.base0 as u32 | (u32::from(d.base1()) << 16) | (u32::from(d.base2()) << 24)
}

pub fn desc_limit(d: &desc_t) -> u16 {
    d.limit0 | (d.limit1() << 16) as u16
}

pub fn desc_ptr(gdt_base: *const u8, idx: usize) -> *const desc_t {
    (gdt_base as usize + idx * std::mem::size_of::<desc_t>()) as *const desc_t
}

// pt.h

pub fn mask_to(m: u64) -> u64 {
    (1 << (m + 1)) - 1
}

pub fn create_mask_ordered(n: u64, m: u64) -> u64 {
    mask_to(m) ^ (mask_to(n) >> 1)
}

pub fn create_mask(n: u64, m: u64) -> u64 {
    if n < m {
        create_mask_ordered(n, m)
    } else {
        create_mask_ordered(m, n)
    }
}

pub unsafe fn m() -> u64 {
    physical_address_width()
}

pub unsafe fn mask_m() -> u64 {
    ((1 as u64) << m()) - 1
}

pub const A_MASK: u64 = 1 << A_SHIFT;
pub const D_MASK: u64 = 1 << D_SHIFT;
pub const W_MASK: u64 = 1 << W_SHIFT;
pub const US_MASK: u64 = 1 << US_SHIFT;
pub const XD_MASK: u64 = 1 << XD_SHIFT;
pub const P_MASK: u64 = 1;
pub const RSVD_MASK: u64 = 1 << RSVD_SHIFT;
pub const PUD_PS_MASK: u64 = 1 << PUD_PS_SHIFT;
pub const PMD_PS_MASK: u64 = 1 << PMD_PS_SHIFT;
pub const PGD_MASK: u64 = (1 << 9 - 1) << PGD_SHIFT;
pub const PUD_MASK: u64 = (1 << 9 - 1) << PUD_SHIFT;
pub const PMD_MASK: u64 = (1 << 9 - 1) << PMD_SHIFT;
pub const PT_MASK: u64 = (1 << 9 - 1) << PT_SHIFT;
pub unsafe fn pt_phys_mask() -> u64 {
    mask_m() << PT_PHYS_SHIFT
}
pub const PAGE_MASK: u64 = (1 << 12 - 1) << PAGE_SHIFT;
pub const PAGE1GiB_MASK: u64 = (1 << 30 - 1) << PAGE1GiB_SHIFT;
pub const PAGE2MiB_MASK: u64 = (1 << 21 - 1) << PAGE2MiB_SHIFT;

pub fn accessed(entry: u64) -> u64 {
    (entry & A_MASK) >> A_SHIFT
}

pub fn mark_accessed(entry: u64) -> u64 {
    entry | A_MASK
}

pub fn mark_not_accessed(entry: u64) -> u64 {
    entry & !A_MASK
}

pub fn dirty(entry: u64) -> u64 {
    (entry & D_MASK) >> D_SHIFT
}

pub fn mark_dirty(entry: u64) -> u64 {
    entry | D_MASK
}

pub fn mark_clean(entry: u64) -> u64 {
    entry & !D_MASK
}

pub fn writable(entry: u64) -> u64 {
    (entry & W_MASK) >> W_SHIFT
}

pub fn mark_writable(entry: u64) -> u64 {
    entry | W_MASK
}

pub fn mark_non_writable(entry: u64) -> u64 {
    entry & !W_MASK
}

pub fn user(entry: u64) -> u64 {
    (entry & US_MASK) >> US_SHIFT
}

pub fn mark_user(entry: u64) -> u64 {
    entry | US_MASK
}

pub fn mark_supervisor(entry: u64) -> u64 {
    entry & !US_MASK
}

pub fn execute_disable(entry: u64) -> u64 {
    (entry & XD_MASK) >> XD_SHIFT
}

pub fn mark_execute_disable(entry: u64) -> u64 {
    entry | XD_MASK
}

pub fn mark_not_execute_disable(entry: u64) -> u64 {
    entry & !XD_MASK
}

pub fn executable(entry: u64) -> u64 {
    if execute_disable(entry) == 1 {
        0
    } else {
        1
    }
}

pub fn mark_executable(entry: u64) -> u64 {
    mark_not_execute_disable(entry)
}

pub fn mark_non_executable(entry: u64) -> u64 {
    mark_execute_disable(entry)
}

pub fn present(entry: u64) -> u64 {
    (entry & P_MASK) >> P_SHIFT
}

pub fn mark_present(entry: u64) -> u64 {
    entry | P_MASK
}

pub fn mark_not_present(entry: u64) -> u64 {
    entry & !P_MASK
}

pub fn rsvd(entry: u64) -> u64 {
    (entry & RSVD_MASK) >> RSVD_SHIFT
}

pub fn mark_rsvd(entry: u64) -> u64 {
    entry | RSVD_MASK
}

pub fn clear_rsvd(entry: u64) -> u64 {
    entry & !RSVD_MASK
}

pub fn pud_ps(entry: u64) -> u64 {
    (entry & PUD_PS_MASK) >> PUD_PS_SHIFT
}

pub fn pmd_ps(entry: u64) -> u64 {
    (entry & PMD_PS_MASK) >> PMD_PS_SHIFT
}

pub unsafe fn pgd_phys(entry: u64) -> u64 {
    entry & create_mask(12, m() - 1)
}

pub unsafe fn pud_ps_0_phys(entry: u64) -> u64 {
    entry & create_mask(12, m() - 1)
}

pub unsafe fn pud_ps_1_phys(entry: u64) -> u64 {
    entry & create_mask(30, m() - 1)
}

pub unsafe fn pmd_ps_0_phys(entry: u64) -> u64 {
    entry & create_mask(12, m() - 1)
}

pub unsafe fn pmd_ps_1_phys(entry: u64) -> u64 {
    entry & create_mask(21, m() - 1)
}

pub unsafe fn pt_phys(entry: u64) -> u64 {
    entry & create_mask(12, m() - 1)
}

pub fn pgd_index(virt: u64) -> u64 {
    (virt & PGD_MASK) >> PGD_SHIFT
}

pub fn pud_index(virt: u64) -> u64 {
    (virt & PUD_MASK) >> PUD_SHIFT
}

pub fn pmd_index(virt: u64) -> u64 {
    (virt & PMD_MASK) >> PMD_SHIFT
}

pub fn pt_index(virt: u64) -> u64 {
    (virt & PT_MASK) >> PT_SHIFT
}

pub fn page_index(virt: u64) -> u64 {
    (virt & PAGE_MASK) >> PAGE_SHIFT
}

pub fn page1gib_index(virt: u64) -> u64 {
    (virt & PAGE1GiB_MASK) >> PAGE1GiB_SHIFT
}

pub fn page2mib_index(virt: u64) -> u64 {
    (virt & PAGE2MiB_MASK) >> PAGE2MiB_SHIFT
}
