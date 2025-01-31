#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kvm_host.h>
#include <linux/kvm.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

static struct proc_dir_entry *proc_entry;

#include <asm/pgtable.h>
#include <asm/pgtable.h>
#include <asm/pgtable.h>



void check_pfn_errors(unsigned long hfn){
	if (is_error_pfn(hfn)){
        	printk(KERN_INFO "ERROR PFN");
        if(is_error_noslot_pfn(hfn)){
                printk(KERN_INFO "ERROR NOSLOT");
        } 
        if(is_sigpending_pfn(hfn)) {
                printk(KERN_INFO "ERROR INTERRUPTED");
        }
        if(is_noslot_pfn(hfn)){
                printk(KERN_INFO "NO SLOT");
        }
        if(KVM_PFN_ERR_FAULT == hfn){
                printk(KERN_INFO "magic error");
        }
	if(KVM_PFN_ERR_HWPOISON == hfn){
		printk(KERN_INFO "hardware poison");
	}
    }
}

static long get_user_page_info(struct mm_struct *mm, unsigned long va)
{
    long ret;
    int locked = 0;
    struct page *pages[1] = { NULL };
    ret = get_user_pages_remote(mm, va, 1, FOLL_GET, pages, NULL);
    if (ret < 0) {
        pr_err("get_user_pages_remote failed with %ld\n", ret);
        return ret;
    }
    if (ret == 0) {
        pr_err("No pages were returned for VA 0x%lx\n", va);
        return 0;
    }
    struct page *page = pages[0];
    unsigned long pfn = page_to_pfn(page);
    phys_addr_t phys = PFN_PHYS(pfn);
    printk(KERN_INFO "Page Found for VA");
    printk(KERN_INFO "Physical Address:0x%llx", (unsigned long long)phys);
    if (PageTransHuge(page)) {
    	printk(KERN_INFO "page is part of THP\n");
    } else if (PageHuge(page)) {
    	printk(KERN_INFO "page is huge\n");
    } else {
    	printk(KERN_INFO "page is not huge page\n");
    }
    put_page(page);
    return ret;
}


int get_is_hugepage(unsigned long gfn, struct kvm *kvm){
    unsigned long hva;
    hva = gfn_to_hva(kvm, gfn);
    long ret;
    int locked = 0;
    struct page *pages[1] = { NULL };
    ret = get_user_pages_remote(kvm->mm, hva, 1, FOLL_GET, pages, NULL);
    if (ret < 0) {
        return 0;
    }
    if (ret == 0) {
        return 0;
    }
    struct page *page = pages[0];
    if (PageTransHuge(page) || PageHuge(page)) {
       	 put_page(page);
	 return 1;
    } else {
	put_page(page);
        return 0;
    }


}

void memslot_iterate(struct kvm_memslots *memslots,struct kvm *kvm) {
    struct kvm_memory_slot *slot;
    int bucket;
     hash_for_each(memslots->id_hash, bucket, slot, id_node[1]) {
        printk("Memslot ID: %d, GFN: %llx, HVA: 0x%lx, NPages: %llu Size in MB: %d\n",
               slot->id, (unsigned long)slot->base_gfn,slot->userspace_addr,slot->npages, slot->npages/256);
	int hugepage_total = 0;
	unsigned long long gfn_final = slot->base_gfn + slot->npages;
	unsigned long long gfn_iter = slot->base_gfn;
	while(gfn_iter<=gfn_final){
		hugepage_total += get_is_hugepage(gfn_iter, kvm);
		gfn_iter++;
	}
	printk("HUGEPAGES:%d",hugepage_total);
    }
}


void iterate_all_memslots(struct kvm *kvm) {
	int i = 0;
	while(true){
		struct kvm_memslots *memslots;
		rcu_read_lock();
		memslots = rcu_dereference(kvm->memslots[i]);
		if(!memslots){
			rcu_read_unlock();
			break;
		}
		printk(KERN_INFO "NODE %d ----------------iter%d \n", memslots->node_idx, i);
		memslot_iterate(memslots,kvm);
		rcu_read_unlock();
		i++;
	}
}



static void print_first_vm_gfn_to_hva(unsigned long gfn_val)
{
    struct kvm *kvm;
    unsigned long hva;
    unsigned long hfn;
    unsigned long hva_hfn;
    gfn_t gfn = (gfn_t)gfn_val;
    
    kvm = list_first_entry_or_null(&vm_list, struct kvm, vm_list);
    if (!kvm) {
        printk(KERN_ERR "No VMs found\n");
        return;
    }
    printk(KERN_INFO "Found VM\n");
    
    hva = gfn_to_hva(kvm, gfn);
    printk(KERN_INFO "GFN 0x%lx maps to HVA 0x%lx\n", gfn_val, hva);
//    hfn = gfn_to_pfn(kvm,gfn);
//    hva_hfn = __gfn_to_pfn_memslot(gfn_to_memslot(kvm,gfn), gfn, false, false, NULL, true,
//				    NULL, NULL);
//    check_pfn_errors(hfn);
//    printk("custom hva");
//    check_pfn_errors(hva_hfn);
    if (kvm_is_error_hva(hva)) {
        printk(KERN_ERR "Error getting HVA for GFN 0x%lx\n", gfn_val);
        return;
    }
//    struct vm_area_struct *vma;
//    vma = vma_lookup(kvm->mm,hva);
//    if(vma == NULL){
//	printk(KERN_INFO "NO VMA");
//    }

    get_user_page_info(kvm->mm,hva);
    iterate_all_memslots(kvm);
}


static ssize_t gfn_write(struct file *file, const char __user *ubuf,
                        size_t count, loff_t *ppos)
{
    char buf[32];
    unsigned long gfn;
    
    if (count > sizeof(buf) - 1)
        return -EINVAL;
        
    if (copy_from_user(buf, ubuf, count))
        return -EFAULT;
        
    buf[count] = '\0';
    if (kstrtoul(buf, 0, &gfn))
        return -EINVAL;
        
    print_first_vm_gfn_to_hva(gfn);
    return count;
}

static const struct proc_ops gfn_fops = {
    .proc_write = gfn_write,
};

static int __init gfn_module_init(void)
{
    proc_entry = proc_create("gfn_to_hva", 0666, NULL, &gfn_fops);
    if (!proc_entry)
        return -ENOMEM;
    printk(KERN_INFO "GFN to HVA module loaded\n");
    return 0;
}

static void __exit gfn_module_exit(void)
{
    proc_remove(proc_entry);
    printk(KERN_INFO "GFN to HVA module unloaded\n");
}

module_init(gfn_module_init);
module_exit(gfn_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("GFN to HVA translation module");
