#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/kvm_host.h>
#include <linux/list.h>
#include <linux/kallsyms.h>

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
	struct kvm_memory_slot *slot;
	unsigned long hpa = 0;

	raw_spin_lock(vm_lock);
	list_for_each_entry(kvm, sum_list, vm_list) {
		slot = gfn_to_memslot(kvm, current_gpn);
		if (slot) {
			hpa = __pa(slot->userspace_addr + ((current_gpn - slot->base_gfn) << PAGE_SHIFT));
			break;
		}
	}
	raw_spin_unlock(vm_lock);

	if (hpa) {
		printk("GPN %llu(0x%llx) maps to HPA %llu(0x%llx)\n", current_gpn,current_gpn, hpa,hpa);
		seq_printf(m, "GPN %llu(0x%llx) maps to HPA %llu(0x%llx)\n", current_gpn,current_gpn, hpa,hpa);
	} else {
		seq_printf(m, "No mapping found for GPN 0x%llx\n", current_gpn);
	}

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
