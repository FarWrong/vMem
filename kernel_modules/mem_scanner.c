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
#include <linux/khugepaged.h>


struct khugepaged_scan {
	struct list_head mm_head;
	struct khugepaged_mm_slot *mm_slot;
	unsigned long address;
};

pmd_t *mm_find_pmd(struct mm_struct *mm, unsigned long address)
{
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd = NULL;

	pgd = pgd_offset(mm, address);
	if (!pgd_present(*pgd))
		goto out;

	p4d = p4d_offset(pgd, address);
	if (!p4d_present(*p4d))
		goto out;

	pud = pud_offset(p4d, address);
	if (!pud_present(*pud))
		goto out;

	pmd = pmd_offset(pud, address);
out:
	return pmd;
}

int pmd_huge(pmd_t pmd)
{
	return !pmd_none(pmd) &&
		(pmd_val(pmd) & (_PAGE_PRESENT|_PAGE_PSE)) != _PAGE_PRESENT;
}


bool scan_pmd(struct mm_struct *mm, struct vm_area_struct *vm, unsigned long address){
	pmd_t *pmd;
	pte_t *pte, *_pte;
	unsigned long _address;
	pmd = mm_find_pmd(mm, address);
	if(!pmd || pmd_none(*pmd))
		return false;
//	if(pmd_huge(*pmd)){
//		return true;
//	}
	return false;
}

struct mm_slot {
	struct hlist_node hash;
	struct list_head mm_node;
	struct mm_struct *mm;
};

void scan_mm_slot(struct mm_slot *slot, int *accum){
	struct vma_iterator vmi;
	struct vm_area_struct *vma;
	unsigned long address = 0;
	vma = NULL;
	struct mm_struct *mm = slot->mm;
	vma_iter_init(&vmi, mm, address);
	for_each_vma(vmi,vma){
		unsigned long hstart, hend;
		address = 0;
		hstart = (vma->vm_start + ~HPAGE_PMD_MASK) & HPAGE_PMD_MASK;
		hend = vma->vm_end & HPAGE_PMD_MASK;
		if (hstart >= hend){
			continue;
		}
		if(address<hstart){
			address = hstart;
		}
		while(address < hend){
			accum += scan_pmd(mm,vma,address);
			address += HPAGE_PMD_SIZE;
		}
	}
}

static void do_full_scan(void){
	int pass_through_head = 0;
	int accum = 0;
	//get scan object
	struct khugepaged_scan *scan = get_khugepaged_scan();
	//all we really want is the head slot;
	struct list_head head = scan->mm_head;
	struct mm_slot *iter_slot = list_entry(head.next,struct mm_slot,mm_node);
	while(iter_slot){
		scan_mm_slot(iter_slot, &accum);
	}
	printk("NUMBER OF HUGE PAGES%d",accum);
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
	do_full_scan();
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
