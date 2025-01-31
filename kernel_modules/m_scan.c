#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kvm_host.h>
#include <linux/kvm.h>
#include <linux/mm.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/memory.h>
static struct proc_dir_entry *proc_entry;
#include <linux/memblock.h>
#include <asm/pgtable.h>
#include <linux/slab.h>
#include <linux/khugepaged.h>
#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/mm_types.h>


struct scan_data {
	long free_pages;
	int invalid_pages;
	int other_pages;
	int huge_pages;
	int used_huge_pages;
	int direct_pages;
	int direct_1gb;
	int direct_2mb;
	int direct_4kb;
	int user_pages;
	int total_pages;
	int slab_pages;
};

struct logic_data {
	bool pmd;
	bool pte;
	bool pud;
	bool unf;
	bool present;
};


#define page_slab(p)		(_Generic((p),				\
	const struct page *:	(const struct slab *)(p),		\
	struct page *:		(struct slab *)(p)))



void scan_zone(struct zone *zone, struct scan_data *scan_data){
	unsigned long start_pfn = zone->zone_start_pfn;
	unsigned long end_pfn = zone_end_pfn(zone);
	unsigned long iter_pfn = start_pfn;
	struct page *page;
	unsigned long vaddr;
	for(;iter_pfn<end_pfn;iter_pfn++){
		struct logic_data log;
		if(!pfn_valid(iter_pfn)){
			scan_data->invalid_pages+=1;
			continue;
		}
		
		page = pfn_to_page(iter_pfn);
		vaddr = (unsigned long)phys_to_virt(iter_pfn<<PAGE_SHIFT);
		if(!page || PageReserved(page)){
//		if(!page){
			scan_data->invalid_pages+=1;
			continue;
		}
		//is this a slab/slub page?
		if(PageSlab(page)){
			unsigned int size  = PAGE_SIZE << folio_order(page_folio(page));
			scan_data->slab_pages++;
			//printk("Size of pg_slab %d",size);
			continue;
		}
//		if(page_mapped(page)){
//			continue;
//		}
		//if (PageBuddy(page)) {
    		//	unsigned int order = page_private(page); // Get the buddy order
    		///	unsigned int buddy_size_in_pages = 1 << order; // Buddy size in pages
    		//	unsigned int size_in_kb = (buddy_size_in_pages * PAGE_SIZE) / 1024; // Convert to KB
    // Add the size to slab_pages
    		//	scan_data->slab_pages += size_in_kb;
    // Log the buddy order and size
//    			printk("Buddy page order: %d, size: %u KB\n", order, size_in_kb);
		}
		scan_data->total_pages++;
		//is this a user hugepage?
		if(PageHuge(page)||PageTransHuge(page) || PageCompound(page)){
			if(page_mapped(page)){
				scan_data->used_huge_pages++;
			}
			scan_data->huge_pages+=1;
			continue;
		}
		if(page_mapped(page)){
			continue;
		}
		scan_data->other_pages+=1;
//        break;
	}
}




static void do_full_scan(void){
	int nid; // node id
	pg_data_t *pgdat;
	int zoneid;
	struct zone *zone;
	struct scan_data scan_data = {0};
	struct logic_data log;
	int level;
	scan_data.direct_4kb = 0;
	scan_data.direct_2mb = 0;
	scan_data.direct_1gb = 0;
	unsigned long count = 0;
	struct list_head *entry;
	struct mm_struct *mm_init;
	mm_init = get_init_mm();
	struct page *page;
	for_each_node_state(nid,N_MEMORY){
		pgdat = NODE_DATA(nid);
		for (zoneid = 0; zoneid <= pgdat->kcompactd_highest_zoneidx; zoneid++) {
			zone = &pgdat->node_zones[zoneid];
			scan_data.free_pages+= atomic_long_read(&zone->vm_stat[NR_FREE_PAGES]);
			//if (!populated_zone(zone))
			//continue;
			scan_zone(zone, &scan_data);
		}
	}
//	scan_data.free_pages
	unsigned long total_valid_kb = (scan_data.total_pages) * (PAGE_SIZE / 1024);
	unsigned long total_size_huge = scan_data.huge_pages * (PAGE_SIZE/1024);
	printk("Total valid memory: %lu KB\n", total_valid_kb);
	printk("Total free pages:%lu Total amount %lu\n",scan_data.free_pages * (PAGE_SIZE/1024),scan_data.free_pages);
	printk("Top slab count %d\n",scan_data.slab_pages*(PAGE_SIZE/1024), scan_data.slab_pages);
	printk("Total used huge memory:%lu KB %d pages\n",scan_data.used_huge_pages * (PAGE_SIZE/1024), scan_data.used_huge_pages);
	printk("Total size:%d\n",scan_data.slab_pages);
	printk("Total user pages:%lu KB %d pages\n",scan_data.user_pages * (PAGE_SIZE/1024), scan_data.user_pages);
	printk("Total invalid pages:%lu KB %d pages\n",scan_data.invalid_pages * (PAGE_SIZE/1024), scan_data.invalid_pages);

}


static ssize_t gfn_write(struct file *file, const char __user *ubuf,
                        size_t count, loff_t *ppos){

	do_full_scan();
    	return 1;
}

static const struct proc_ops gfn_fops = {
    .proc_write = gfn_write,
};

static int __init gfn_module_init(void)
{
    proc_entry = proc_create("m_scan", 0666, NULL, &gfn_fops);
    if (!proc_entry)
        return -ENOMEM;
    printk(KERN_INFO "m_scan loaded\n");
    return 0;
}

static void __exit gfn_module_exit(void)
{
    proc_remove(proc_entry);
    printk(KERN_INFO "m_scan unloaded\n");
}

module_init(gfn_module_init);
module_exit(gfn_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("m_scan module");

