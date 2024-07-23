// Linux Kernel/LKM headers: module.h is needed by all modules and kernel.h is needed for KERN_INFO.
#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>   // included for KERN_INFO
#include <linux/init.h>     // included for __init and __exit macros
#include <linux/kallsyms.h>
#include <linux/string.h>
#include <linux/kvm.h>
#include <linux/kvm_host.h>
#include <linux/mm_types.h>
#include <linux/slab.h>
#include <linux/async.h>

MODULE_LICENSE("GPL");

static int __init kvm_enum_init(void)  {
    struct list_head *sum_list;
    struct kvm *kvm;
    int count = 0;
    struct kvm_memory_slot *slot;
    raw_spinlock_t *vm_lock;
    printk(KERN_INFO "KVM Instance Enumeration Module loaded\n");
    u64 gpn = 30;
    // Look up the address of vm_list
    sum_list = (struct list_head*)kallsyms_lookup_name("vm_list");
    vm_lock =(raw_spinlock_t*)kallsyms_lookup_name("kvm_lock");
//    if (!vm_list) {
//        printk(KERN_ERR "Failed to find vm_list symbol\n");
//        return -EINVAL;
//    }
    raw_spin_lock(vm_lock);
    list_for_each_entry(kvm, sum_list, vm_list)
    {
	slot = gfn_to_memslot(kvm, gpn);
    	if (slot) {
        	printk(KERN_INFO "Found memory slot for GPN %llu\n", (unsigned long long)gpn);
    	} else {
        	printk(KERN_INFO "No memory slot found for GPN %llu\n", (unsigned long long)30);
    	}
        printk(KERN_INFO "testy mc test\n", kvm);

    }
    raw_spin_unlock(vm_lock);


    printk(KERN_INFO "Total KVM instances found: %d\n", count);

    return 0;
}

static void __exit kvm_enum_exit(void)
{
    printk(KERN_INFO "KVM Instance Enumeration Module unloaded\n");
}

module_init(kvm_enum_init);
module_exit(kvm_enum_exit);
