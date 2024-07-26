#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kvm_host.h>
#include <linux/list.h>
#include <linux/kallsyms.h>
#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <asm/pgtable.h>
#include <asm/set_memory.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("KVM GPN to HPA Converter");

static struct proc_dir_entry *gpn_to_hpa_entry;
static u64 current_gpn = 0;
static struct list_head *sum_list;
static raw_spinlock_t *vm_lock;

static int gpn_to_hpa_show(struct seq_file *m, void *v)
{
    struct kvm *kvm;
    unsigned long hpa = 0;
    int is_backed = 0;
    int is_huge = 0;
    u64 pfn = 0;
    unsigned long hva = 0;
    struct vm_area_struct *vma;
    pte_t *pte;
    unsigned int level = 0;
    struct page *page;
    int ret;
    spinlock_t *ptl;

    raw_spin_lock(vm_lock); // Ensure proper lock initialization
    list_for_each_entry(kvm, sum_list, vm_list) {
        struct mm_struct *mm = kvm->mm;
        
        // Ensure current_gpn is valid and convert to pfn and hva
        hva = gfn_to_hva(kvm, current_gpn);

        pfn = gfn_to_pfn(kvm, current_gpn);
        kvm_release_pfn_clean(pfn);
        if (pfn == -1UL) {
            printk(KERN_ERR "Invalid GFN to PFN conversion.\n");
            continue; // Skip this entry
        }

        mmap_read_lock(mm);   
        follow_pte(mm, hva, &pte, &ptl);
         spin_lock(ptl);
            seq_printf(m, "GPN: 0x%llx\n", current_gpn);
            seq_printf(m, "HVA: 0x%lx\n", hva);
            seq_printf(m, "HPA: 0x%lx\n", hpa);
            spin_unlock(ptl);
        mmap_read_unlock(mm);
        
        // Output data
        
    }
    raw_spin_unlock(vm_lock);
    printk("GPN %llu, Valid Host Page:%d IsHuge:%d HostPageNumber:%llu HVA:%lu\n", 
                   current_gpn, is_backed, is_huge, pfn, hva);
    seq_printf(m, "GPN %llu, Valid Host Page:%d IsHuge:%d HostPageNumber:%llu HVA:%lu\n", 
                   current_gpn, is_backed, is_huge, pfn, hva);
    return 0;
}
static ssize_t gpn_to_hpa_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	char buf[20];
	u64 gpn;

	if (count > sizeof(buf) - 1)
		return -EINVAL;

	if (copy_from_user(buf, user_buf, count))
		return -EFAULT;

	buf[count] = '\0';
	if (kstrtoull(buf, 0, &gpn) < 0)
		return -EINVAL;

	current_gpn = gpn;
	return count;
}

static int gpn_to_hpa_open(struct inode *inode, struct file *file)
{
	return single_open(file, gpn_to_hpa_show, NULL);
}

static const struct proc_ops gpn_to_hpa_fops = {
	.proc_open = gpn_to_hpa_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
	.proc_write = gpn_to_hpa_write,
};

static int __init kvm_gpn_to_hpa_init(void)
{
	printk(KERN_INFO "KVM GPN to HPA Converter loaded\n");

	sum_list = (struct list_head*)kallsyms_lookup_name("vm_list");
	vm_lock = (raw_spinlock_t*)kallsyms_lookup_name("kvm_lock");

	if (!sum_list || !vm_lock) {
		printk(KERN_ERR "Failed to find required symbols\n");
		return -EINVAL;
	}

	gpn_to_hpa_entry = proc_create("kvm_gpn_to_hpa", 0644, NULL, &gpn_to_hpa_fops);
	if (!gpn_to_hpa_entry) {
		printk(KERN_ERR "Failed to create proc entry\n");
		return -ENOMEM;
	}

	return 0;
}

static void __exit kvm_gpn_to_hpa_exit(void)
{
	proc_remove(gpn_to_hpa_entry);
	printk(KERN_INFO "KVM GPN to HPA Converter unloaded\n");
}

module_init(kvm_gpn_to_hpa_init);
module_exit(kvm_gpn_to_hpa_exit);
