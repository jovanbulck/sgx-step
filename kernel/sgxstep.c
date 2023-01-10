/*
 *  This file is part of the SGX-Step enclave execution control framework.
 *
 *  Copyright (C) 2017 Jo Van Bulck <jo.vanbulck@cs.kuleuven.be>,
 *                     Raoul Strackx <raoul.strackx@cs.kuleuven.be>
 *
 *  SGX-Step is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SGX-Step is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SGX-Step. If not, see <http://www.gnu.org/licenses/>.
 */

#include "sgxstep_internal.h"
#include "sgxstep_ioctl.h"

#include <asm/pgtable.h>
#include <asm/page.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <asm/irq.h>

#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/kprobes.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/slab.h>

#include <linux/clockchips.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jo Van Bulck <jo.vanbulck@cs.kuleuven.be>, Raoul Strackx <raoul.strackx@cs.kuleuven.be>");
MODULE_DESCRIPTION("SGX-Step: A Practical Attack Framework for Precise Enclave Execution Control");

static struct page **isr_pages = NULL;
static uint64_t isr_nr_pages = 0;
static void *isr_kernel_vbase = NULL;

static int in_use = 0;

typedef struct {
    uint16_t size;
    uint64_t base;
} __attribute__((packed)) dtr_t;

static dtr_t original_dtr = {0};

static void *original_idt_copy = NULL;

int step_open(struct inode *inode, struct file *file)
{
    if (in_use)
    {   
        err("Device is already opened");
        return -EBUSY;
    }

    asm volatile ("sidt %0\n\t"
                  :"=m"(original_dtr) :: );

    log("read idt: 0x%llx with size %u", original_dtr.base, original_dtr.size+1);

    original_idt_copy = kmalloc(original_dtr.size+1, GFP_KERNEL);
    RET_ASSERT(original_idt_copy);

    memcpy(original_idt_copy, (void*)original_dtr.base, original_dtr.size+1);
    log("copied original idt");

    in_use = 1;
    return 0;
}

int step_release(struct inode *inode, struct file *file)
{
    uint64_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));

    // disable write protection
    asm volatile("mov %0, %%cr0" : : "r"(cr0 & ~(1llu << 16)));

    // restore original idt to ensure no user pointers are left
    memcpy((void*)original_dtr.base, original_idt_copy, original_dtr.size+1);
    log("restored original idt at 0x%llx with size %u", original_dtr.base, original_dtr.size+1);

    // restore original cr0
    asm volatile("mov %0, %%cr0" : : "r"(cr0));

    kfree(original_idt_copy);
    original_idt_copy = NULL;

    if (isr_kernel_vbase) {
        // freeup the kernel vbase mapping for isrs
        vunmap(isr_kernel_vbase);
        
        // unpin the isr user physical pages
        unpin_user_pages(isr_pages, isr_nr_pages);
        
        // free the isr user physical page structure
        kfree(isr_pages);

        isr_kernel_vbase = NULL;
        isr_pages = NULL;
        isr_nr_pages = 0;
    }

    in_use = 0;
    return 0;
}

/* Convenience function when editing PTEs from user space (but normally not
 * needed, since SGX already flushes the TLB on enclave entry/exit) */
long sgx_step_ioctl_invpg(struct file *filep, unsigned int cmd, unsigned long arg)
{
    uint64_t addr = ((invpg_t *) arg)->adrs;

    asm volatile("invlpg (%0)" ::"r" (addr) : "memory");

    return 0;
}

long sgx_step_get_pt_mapping(struct file *filep, unsigned int cmd, unsigned long arg)
{
    address_mapping_t *map = (address_mapping_t*) arg;
	pgd_t *pgd = NULL;
	pud_t *pud = NULL;
	pmd_t *pmd = NULL;
	pte_t *pte = NULL;
    #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0))
        p4d_t *p4d = NULL;
    #endif

	uint64_t virt;
    RET_ASSERT(map);
	
	virt = map->virt;
	memset( map, 0x00, sizeof( address_mapping_t ) );
	map->virt = virt;
	
	map->pgd_phys_address = __pa( current->mm->pgd );
	pgd = pgd_offset( current->mm, virt );
	map->pgd = *((uint64_t *) pgd);
	
	if ( !pgd_present( *pgd ) )
		return 0;

    #if (LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0))
        #if CONFIG_PGTABLE_LEVELS > 4
            #warning 5-level page tables currently not supported by SGX-Step
            #warning unfolding dummy p4d; try rebooting with `no5lvl` kernel parameter if needed
        #endif

        /* simply unfold the pgd inside the dummy p4d struct */
        p4d = p4d_offset( pgd, virt);
        pud = pud_offset( p4d, virt );
    #else
        pud = pud_offset( pgd, virt );
    #endif

	map->pud = *((uint64_t *) pud);
	
	if ( !pud_present( *pud ) )
		return 0;
	
	pmd = pmd_offset( pud, virt );
	map->pmd = *((uint64_t *) pmd);
	
	if ( !pmd_present( *pmd ) )
		return 0;
	
	pte = pte_offset_map( pmd, virt );
	map->pte = *((uint64_t *) pte);
	
	if ( !pte_present( *pte ) )
		return 0;
	
	map->phys = PFN_PHYS( pte_pfn( *pte ) ) | ( virt & 0xfff );

    return 0;
}


long sgx_step_ioctl_setup_isr_map(struct file *filep, unsigned int cmd, unsigned long arg)
{
    uint64_t nr_pinned_pages;

    setup_isr_map_t *data = (setup_isr_map_t*) arg;
    
    isr_nr_pages = (data->isr_stop - data->isr_start + PAGE_SIZE - 1) / PAGE_SIZE;

    isr_pages = kmalloc(isr_nr_pages * sizeof(struct page *), GFP_KERNEL);
    RET_ASSERT_GOTO(isr_pages, "cannot allocate memory", out);

    nr_pinned_pages = pin_user_pages(data->isr_start & ~(PAGE_SIZE - 1), isr_nr_pages, FOLL_LONGTERM | FOLL_WRITE, isr_pages, NULL);
    log("nr_pinned_pages = %llu should be %llu", nr_pinned_pages, isr_nr_pages);

    RET_ASSERT_GOTO(nr_pinned_pages == isr_nr_pages, "could not pin all isr pages", cleanup_pages);

    isr_kernel_vbase = vmap(isr_pages, isr_nr_pages, VM_READ | VM_EXEC | VM_SHARED, PAGE_SHARED_EXEC);
    RET_ASSERT_GOTO(isr_kernel_vbase, "could not vmap isr", cleanup_pin);

    log("mapped isr to kernel virtual address 0x%llx", (uint64_t)isr_kernel_vbase);
    
    data->isr_kernel_base = isr_kernel_vbase;
    
    return 0;

cleanup_pin:
    unpin_user_pages(isr_pages, isr_nr_pages);
    
cleanup_pages:
    kfree(isr_pages);

out:
    isr_kernel_vbase = NULL;
    isr_pages = NULL;
    isr_nr_pages = 0;
    return -EINVAL;
}


typedef long (*ioctl_t)(struct file *filep, unsigned int cmd, unsigned long arg);

long step_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    char data[256];
    ioctl_t handler = NULL;
    long ret;

    switch (cmd)
    {
        case SGX_STEP_IOCTL_GET_PT_MAPPING:
            handler = sgx_step_get_pt_mapping;
            break;
        case SGX_STEP_IOCTL_INVPG:
            handler = sgx_step_ioctl_invpg;
            break;
        case SGX_STEP_IOCTL_SETUP_ISR_MAP:
            handler = sgx_step_ioctl_setup_isr_map;
            break;
        default:
            return -EINVAL;
    }

    RET_ASSERT(handler && (_IOC_SIZE(cmd) < 256));
    if (copy_from_user(data, (void __user *) arg, _IOC_SIZE(cmd)))
        return -EFAULT;

    ret = handler(filep, cmd, (unsigned long) ((void *) data));

    if (!ret && (cmd & IOC_OUT)) {
        if (copy_to_user((void __user *) arg, data, _IOC_SIZE(cmd)))
            return -EFAULT;
    }

    return 0;
}

static const struct file_operations step_fops = {
    .owner              = THIS_MODULE,
    .compat_ioctl       = step_ioctl,
    .unlocked_ioctl     = step_ioctl,
    .open               = step_open,
    .release            = step_release
};

static struct miscdevice step_dev = {
    .minor  = MISC_DYNAMIC_MINOR,
    .name   = DEV,
    .fops   = &step_fops,
    .mode   = S_IRUGO | S_IWUGO
};

/* Code from: <https://www.libcrack.so/index.php/2012/09/02/bypassing-devmem_is_allowed-with-kprobes/> */
static int devmem_is_allowed_handler (struct kretprobe_instance *rp, struct pt_regs *regs)
{
    if (regs->ax == 0) {
        regs->ax = 0x1;
    }
    return 0;
}

static struct kretprobe krp = {
    .handler = devmem_is_allowed_handler,
    .maxactive = 20 /* Probe up to 20 instances concurrently. */
};

int init_module(void)
{
    /* Register virtual device */
    if (misc_register(&step_dev))
    {
        err("virtual device registration failed..");
        step_dev.this_device = NULL;
        return -EINVAL;
    }

    /* Activate a kretprobe to bypass CONFIG_STRICT_DEVMEM kernel compilation option */
    krp.kp.symbol_name = "devmem_is_allowed";
    if (register_kretprobe(&krp) < 0)
    {
        err("register_kprobe failed..");
        step_dev.this_device = NULL;
        return -EINVAL;
    }

    log("listening on /dev/" DEV);
    return 0;
}

void cleanup_module(void)
{
    /* Unregister virtual device */
    if (step_dev.this_device)
        misc_deregister(&step_dev);


    unregister_kretprobe(&krp);
    log("kernel module unloaded");
}
