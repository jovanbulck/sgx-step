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

#include <linux/kallsyms.h>
#include <linux/clockchips.h>
#include <asm/apic.h>
#include <asm/apicdef.h>

#include "linux-sgx-driver/sgx.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jo Van Bulck <jo.vanbulck@cs.kuleuven.be>, Raoul Strackx <raoul.strackx@cs.kuleuven.be>");
MODULE_DESCRIPTION("SGX-Step: A Practical Attack Framework for Precise Enclave Execution Control");

int target_cpu = -1;
struct clock_event_device *levt = NULL;
void (*event_handler)(struct clock_event_device *) = NULL;
struct sgx_step_enclave_info victim = {0};

typedef long (*apvm_t)(struct task_struct *tsk, unsigned long addr, void *buf, int len, int write);

long edbgrd(unsigned long addr, void *buf, int len)
{
    apvm_t apvm;

    /* access_process_vm will use the vm_operations defined by the isgx driver */
    RET_ASSERT(apvm = (apvm_t) kallsyms_lookup_name("access_process_vm"));
    return apvm(current, addr, buf, len, /*write=*/0);
}

unsigned long edbgrd_ssa(unsigned long tcs_addr, int ssa_field_offset)
{
    unsigned long ossa, ret, ssa_field_addr;

    edbgrd(tcs_addr + SGX_TCS_OSSA_OFFSET, &ossa, 8);
    ssa_field_addr = victim.base + ossa + SGX_SSAFRAMESIZE
                     - SGX_GPRSGX_SIZE + ssa_field_offset;

    edbgrd(ssa_field_addr, &ret, 8);
    return ret;
}

int get_erip(void)
{
    struct pt_regs *regs = task_pt_regs(current);
    unsigned long erip = 0x0;

    RET_ASSERT(regs && victim.aep && victim.erip_pt);
    if (regs->ip == victim.aep)
    {
        /* NOTE: we do not pass regs->rbx (which should contain &TCS), but
           rather use the value provided by the untrusted runtime; for
           regs->rbx somehow doesn't contain the right value here.. */
        erip = edbgrd_ssa(victim.tcs, SGX_GPRSGX_RIP_OFFSET);
        RET_ASSERT(!copy_to_user((void __user *) victim.erip_pt,
                                 &erip, sizeof(erip)));
    }
    else
    {
        log("interrupted non-enclave code at %p", (void*) regs->ip);
    }

    return 0;
}

void attacker_lapic_event_handler(struct clock_event_device *dev)
{
    BUG_ON(!event_handler);

    if (victim.aep && victim.erip_pt) get_erip();

    /* XXX insert custom kernel-level, attack-specific code here */

    event_handler(dev);
}

void attacker_ipi_handler(void)
{
    if (smp_processor_id() != target_cpu)
        return;

    if (victim.aep && victim.erip_pt) get_erip();

    /* XXX insert custom kernel-level, attack-specific code here */
}

int step_open(struct inode *inode, struct file *file)
{
    if (target_cpu != -1)
    {   
        err("Device is already opened");
        return -EBUSY;
    }
    target_cpu = smp_processor_id();

    /* Hook IPI vector (cf. https://github.com/jovanbulck/sgx-pte) */
    kvm_set_posted_intr_wakeup_handler(attacker_ipi_handler);
    return 0;
}

int step_release(struct inode *inode, struct file *file)
{
    target_cpu = -1;

    /* Restore local APIC timer IRQ handler */
    if (levt && event_handler)
    {
        levt->event_handler = event_handler;
        log("local APIC timer event handler %p restored", event_handler);

        // genuine handler puts kernel back in control of APIC timer deadline
        event_handler(levt);
        levt = NULL; event_handler = NULL;
    }

    /* Unhook IPI vector */
    kvm_set_posted_intr_wakeup_handler(NULL);
    return 0;
}

long sgx_step_ioctl_info(struct file *filep, unsigned int cmd, unsigned long arg)
{
    struct sgx_encl *enclave;
    struct vm_area_struct *vma = NULL;
    struct sgx_step_enclave_info *info = (struct sgx_step_enclave_info *) arg;

    vma = find_vma(current->mm, (uint64_t) info->tcs);
    RET_ASSERT(vma && (enclave = vma->vm_private_data));
    RET_ASSERT(info->aep && info->tcs);

    victim.aep               = info->aep;
    victim.tcs               = info->tcs;
    victim.erip_pt           = info->erip_pt;
    info->base = victim.base = enclave->base;
    info->size = victim.size = enclave->size;

    return 0;
}

/* Wrap local APIC timer IRQ handler */
long sgx_step_ioctl_lapic_hook(struct file *filep, unsigned int cmd, unsigned long arg)
{
    void *lapic_events = NULL;
    RET_ASSERT(lapic_events = (void*) kallsyms_lookup_name("lapic_events"));
    RET_ASSERT(levt = this_cpu_ptr(lapic_events));
    RET_ASSERT(!event_handler && (event_handler = levt->event_handler));
    levt->event_handler = attacker_lapic_event_handler;
    log("CPU %d local APIC timer IRQ handler hooked: %p (original); " \
        "%p (wrapped)", smp_processor_id(), event_handler, levt->event_handler);

    return 0;
}

long sgx_step_ioctl_edbgrd(struct file *filep, unsigned int cmd, unsigned long arg)
{
    edbgrd_t *data = (edbgrd_t*) arg;
    uint8_t buf[data->len];
    edbgrd((unsigned long) data->adrs, &buf, data->len);

    if (copy_to_user((void __user *) data->val, buf, data->len))
        return -EFAULT;
    return 0;
}

long sgx_step_ioctl_invpg(struct file *filep, unsigned int cmd, unsigned long arg)
{
    unsigned long val = 0;

    asm volatile("mov %%cr3, %0\n\t"
                 "mov %0, %%cr3\n\t"
                 : : "r" (val));

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
	
	virt = (uint64_t) map->virt;
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
        case SGX_STEP_IOCTL_LAPIC_HOOK:
            handler = sgx_step_ioctl_lapic_hook;
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

    /* Restore local APIC timer IRQ handler */
    if (levt && event_handler)
    {
        levt->event_handler = event_handler;
        log("local APIC timer event handler %p restored", event_handler);
    }

    unregister_kretprobe(&krp);
    kvm_set_posted_intr_wakeup_handler(NULL);
    log("kernel module unloaded");
}
