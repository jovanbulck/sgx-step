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

#include <linux/clockchips.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0))
    #include <linux/kernel.h>
    #include <linux/module.h>
#else
    #include <linux/kallsyms.h>
#endif

#if !NO_SGX
    #include "linux-sgx-driver/sgx.h"
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jo Van Bulck <jo.vanbulck@cs.kuleuven.be>, Raoul Strackx <raoul.strackx@cs.kuleuven.be>");
MODULE_DESCRIPTION("SGX-Step: A Practical Attack Framework for Precise Enclave Execution Control");

int target_cpu = -1;

typedef long (*apvm_t)(struct task_struct *tsk, unsigned long addr, void *buf, int len, unsigned int write);

// This workaround for the lack of kallsyms_lookup_name was taken from https://github.com/zizzu0/LinuxKernelModules
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0))
    #define KPROBE_PRE_HANDLER(fname) static int __kprobes fname(struct kprobe *p, struct pt_regs *regs)
    #define kallsyms_lookup_name(fname) kln_pointer(fname)

    long unsigned int kln_addr = 0;
    unsigned long (*kln_pointer)(const char *name) = NULL;

    static struct kprobe kp0, kp1;

    KPROBE_PRE_HANDLER(handler_pre0)
    {
        kln_addr = (--regs->ip);
        
        return 0;
    }

    KPROBE_PRE_HANDLER(handler_pre1)
    {
        return 0;
    }

    static int do_register_kprobe(struct kprobe *kp, char *symbol_name, void *handler)
    {
        int ret;
        
        kp->symbol_name = symbol_name;
        kp->pre_handler = handler;
        
        ret = register_kprobe(kp);
        if (ret < 0) {
            err("register_probe() for symbol %s failed, returned %d", symbol_name, ret);
            return ret;
        }
        
        log("Planted kprobe for symbol %s at %p", symbol_name, kp->addr);
        
        return ret;
    }

    int kallsym_init(void) {
        int ret;
        
        ret = do_register_kprobe(&kp0, "kallsyms_lookup_name", handler_pre0);
        if (ret < 0)
            return ret;
        
        ret = do_register_kprobe(&kp1, "kallsyms_lookup_name", handler_pre1);
        if (ret < 0) {
            unregister_kprobe(&kp0);
            return ret;
        }
        
        unregister_kprobe(&kp0);
        unregister_kprobe(&kp1);
        
        log("kallsyms_lookup_name address = %#lx", kln_addr);
        
        kln_pointer = (unsigned long (*)(const char *name)) kln_addr;
        
        log("kallsyms_lookup_name address = %#lx\n", kln_pointer("kallsyms_lookup_name"));

        return 0;
    }

#endif




int step_open(struct inode *inode, struct file *file)
{
    if (target_cpu != -1)
    {   
        err("Device is already opened");
        return -EBUSY;
    }
    target_cpu = smp_processor_id();

    return 0;
}

int step_release(struct inode *inode, struct file *file)
{
    target_cpu = -1;

    return 0;
}

long sgx_step_ioctl_info(struct file *filep, unsigned int cmd, unsigned long arg)
{
    #if !NO_SGX
        struct sgx_encl *enclave;
        struct vm_area_struct *vma = NULL;
        struct sgx_step_enclave_info *info = (struct sgx_step_enclave_info *) arg;

        vma = find_vma(current->mm, (uint64_t) info->tcs);
        RET_ASSERT(vma && (enclave = vma->vm_private_data));
        RET_ASSERT(info->aep && info->tcs);

        info->base = enclave->base;
        info->size = enclave->size;
    #endif

    return 0;
}

long edbgrdwr(unsigned long addr, void *buf, int len, int write)
{
    apvm_t apvm;

    /* access_process_vm will use the vm_operations defined by the isgx driver */
    RET_ASSERT(apvm = (apvm_t) kallsyms_lookup_name("access_process_vm"));
    return apvm(current, addr, buf, len, write);
}

long sgx_step_ioctl_edbgrd(struct file *filep, unsigned int cmd, unsigned long arg)
{
    long ret_status = 0;
    edbgrd_t *data = (edbgrd_t*) arg;
    uint8_t *buf = NULL;
    
    if (data->len > PAGE_SIZE) {
        err("Error allocating more than a page size (%ldB) of memory is currently not supported..", PAGE_SIZE);
        // Note that 'kree(NULL)' executes gracefully 
        ret_status = -ENOMEM;
        goto func_cleanup;
    }

    buf = (uint8_t *)kmalloc_array(data->len, sizeof(uint8_t), GFP_KERNEL);
    if (!buf) {
        err("Could not allocate kernel buffer..");
        ret_status = -ENOMEM;
        goto func_cleanup;
    }

    if (data->write && copy_from_user(buf, (void __user *) data->val, data->len)) {
        ret_status = -EFAULT;
        goto func_cleanup;
    }

    edbgrdwr((unsigned long) data->adrs, buf, data->len, data->write);

    if (!data->write && copy_to_user((void __user *) data->val, (void *)buf, data->len)) {
        ret_status = -EFAULT;
        goto func_cleanup;
    }

func_cleanup:
    kfree(buf);
    return ret_status;
}

typedef void (*ftlb_t)(void);

long sgx_step_ioctl_invpg(struct file *filep, unsigned int cmd, unsigned long arg)
{
    ftlb_t ftlb;
    RET_ASSERT(ftlb = (ftlb_t) kallsyms_lookup_name("flush_tlb_all"));

    ftlb();
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
            #error 5-level page tables currently not supported by SGX-Step
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

typedef long (*ioctl_t)(struct file *filep, unsigned int cmd, unsigned long arg);

long step_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    char data[256];
    ioctl_t handler = NULL;
    long ret;

    switch (cmd)
    {
        case SGX_STEP_IOCTL_VICTIM_INFO:
            handler = sgx_step_ioctl_info;
            break;
        case SGX_STEP_IOCTL_GET_PT_MAPPING:
            handler = sgx_step_get_pt_mapping;
            break;
        case SGX_STEP_IOCTL_EDBGRD:
            handler = sgx_step_ioctl_edbgrd;
            break;
        case SGX_STEP_IOCTL_INVPG:
            handler = sgx_step_ioctl_invpg;
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

/* Code from: <https://www.libcrack.so/2012/09/02/bypassing-devmem_is_allowed-with-kprobes/> */
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

    #if (LINUX_VERSION_CODE >= KERNEL_VERSION(5,7,0))
    if (kallsym_init()) {
        err("Could not initialize kallsym..");
        step_dev.this_device = NULL;
        return -EINVAL;
    }
    #endif

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
