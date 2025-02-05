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
#include <linux/hashtable.h>



#define MSC_HASH_BITS 20

enum use_types {
  USER,
  KERNEL,
  OTHER,
};

enum kernel_classes{
	NONE,
	KERNEL_TEXT,
	KERNEL_MODULES,
	KOTHER
};




struct two_way_page {
	struct hlist_node node;
	unsigned long pfn;
	int use_type;
	bool is_huge;
	bool is_free;
	unsigned long highest_order;
	int kernel_class;
	bool is_present;
	bool is_mapped;
	//now for the host data
	unsigned long h_pfn;
	unsigned long h_highest_order;
	bool h_is_huge;
	bool h_is_present;
	bool h_is_mapped;
};


DEFINE_HASHTABLE(m_scan_tb, MSC_HASH_BITS);

static struct two_way_page *hash_lookup(unsigned long key) {
    struct two_way_page *entry;
    u32 hash = hash_long(key,MSC_HASH_BITS);
    // Iterate over possible entries in the hash bucket
    hash_for_each_possible(m_scan_tb, entry, node, hash) {
        if (entry->pfn == key) {
            return entry; // Found the item
        }
    }
    return NULL; // Not found
}


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


bool is_page_mapped(unsigned long flags) {
    return (flags & (1 << 1)) != 0;
}


bool is_huge_page(unsigned long flags) {
    return (flags & (1 << 0)) != 0;
}

unsigned long get_huge_page_order(unsigned long flags) {
    if (!is_huge_page(flags)) {
        return 0;  // Not a huge page, order is irrelevant.
    }
    return (flags >> 2) & 0x1F; // Extract the 5-bit order field.
}



static inline void kvm_hypercall_two_returns(unsigned long *val1, unsigned long *val2, 
                                           unsigned long input)
{
    register unsigned long rdx asm("rdx");
    register unsigned long rsi asm("rsi");
    kvm_hypercall1(42, input);
    *val2 = rdx;
    *val1 = rsi;
}


int get_order_from_level(int level){
	if(level == PG_LEVEL_4K){
		return 0;
	}else if (level == PG_LEVEL_2M){
		return 1;
	}else if (level == PG_LEVEL_1G){
		return 2;
	}else{
		return -1;
	}
}


struct two_way_page *perform_lookup_on_pfn(unsigned long pfn){
	struct two_way_page *entry;
	entry = kmalloc(sizeof(*entry), GFP_KERNEL);
	if(!entry){
		printk("AHHHHHHHHHH");
	}
	entry->pfn = pfn;
	unsigned long h_pfn;
	unsigned long flags;
	kvm_hypercall_two_returns(&flags, &h_pfn, (pfn << PAGE_SHIFT));
	entry->is_huge = 0;
	entry->h_pfn = h_pfn;
	entry->h_is_huge = is_huge_page(flags);
	entry->h_is_mapped = is_page_mapped(flags);
	entry->h_highest_order = get_huge_page_order(flags);
	entry->use_type = OTHER;
	entry->kernel_class = KOTHER;
	return entry;
}


void scan_kernel_text(void){
	unsigned long iter = 0;
	unsigned long phys = 0;	
	int count = 0;
	int huge_pg=0;
	unsigned long pfn;
	int level = 0;
	for(;iter<KERNEL_IMAGE_SIZE;iter+=PAGE_SIZE){
		phys = __phys_addr(iter + __START_KERNEL_map);
                pfn = phys >> PAGE_SHIFT;
		pte_t *pte = lookup_address(iter+__START_KERNEL_map, &level);
                struct two_way_page *curr = hash_lookup(pfn);
                if(!curr){
			printk("NEW KEY: pfn %lu",pfn);
			u32 hash = hash_long(pfn, MSC_HASH_BITS);
                        curr = perform_lookup_on_pfn(pfn);
                        hash_add(m_scan_tb,&curr->node,hash);
                }
                curr->use_type = KERNEL;
                curr->kernel_class = KERNEL_TEXT;
		curr->highest_order = get_order_from_level(level);
		if(level>0){
                        curr->is_huge = 1;
                }

	}
	printk("huge page:%d , non huge page %d",huge_pg,count);
	return;
}


void scan_kernel_modules(void){
	unsigned long iter = MODULES_VADDR;
	unsigned long phys = 0;	
	unsigned long pfn = 0;
	int count = 0;
	int level = 0;
	for(;iter<MODULES_END;iter+=PAGE_SIZE){
		phys = __phys_addr(iter);
		pfn = phys >> PAGE_SHIFT;
		pte_t *pte = lookup_address(iter, &level);
		struct two_way_page *curr = hash_lookup(pfn);
		if(!curr){
			curr = perform_lookup_on_pfn(pfn);
			u32 hash = hash_long(pfn, MSC_HASH_BITS);
			hash_add(m_scan_tb,&curr->node,hash);
		}
		curr->use_type = KERNEL;
		curr->kernel_class = KERNEL_MODULES;
		curr->highest_order = get_order_from_level(level);
		if(level>0){
			curr->is_huge = 1;
		}
	}
	printk("Amount of pfns in kernel Modules:%d",count);	
	return;
}


void scan_zone(struct zone *zone, struct scan_data *scan_data) {
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
                        continue;
                }
		struct two_way_page *curr = hash_lookup(iter_pfn);
		if(!curr){
			curr = perform_lookup_on_pfn(iter_pfn);
			u32 hash = hash_long(iter_pfn, MSC_HASH_BITS);
                        hash_add(m_scan_tb,&curr->node,hash);
		}
                scan_data->total_pages++;
                //is this a user hugepage?
		if(page_mapped(page)){
			curr->is_mapped = 1;
		}
                if(PageHuge(page)||PageTransHuge(page) || PageCompound(page)){
			curr->is_huge = 1;
			curr->highest_order = folio_order(page_folio(page));
                }
		if(folio_mapped(page_folio(page))){
			curr->use_type = USER;
		}else{
			curr->use_type = KERNEL;
			curr->kernel_class = OTHER;
		}
                if (PageBuddy(page)) {
                        unsigned int order = page_private(page); // Get the buddy order
                        //unsigned int buddy_size_in_pages = 1 << order; // Buddy size in pages
                       	curr->is_free = 1;
			curr->highest_order = order;
                }
        }
}

unsigned long pfn_size(unsigned long pfn_count) {
    return (pfn_count * PAGE_SIZE) / 1024; // Convert bytes to KB
}

void scan_hashmap_stats(void) {
    struct two_way_page *entry;
    struct hlist_node *tmp;
    int bkt;
    unsigned long total_pfn_count = 0;
    unsigned long mismatched_huge_pages = 0;

    hash_for_each_safe(m_scan_tb, bkt, tmp, entry, node) {
        total_pfn_count++; // Count total entries

        // Check for mismatch in huge page status
	//if(entry->use_type == KERNEL && entry->kernel_class == KERNEL_TEXT){
        	if ((entry->h_is_huge !=1) && (entry->is_huge == 1)) {
            		mismatched_huge_pages++;
        	}
	//}
    }

    printk(KERN_INFO "Total PFNs in hash table: %lu\n Size%lu", total_pfn_count,pfn_size(total_pfn_count));
    printk(KERN_INFO "Mismatched huge pages: %lu\n", mismatched_huge_pages);
}



void free_hashmap(void) {
    struct two_way_page *entry;
    struct hlist_node *tmp;
    int bkt;

    /* Iterate over all hash table buckets */
    hash_for_each_safe(m_scan_tb, bkt, tmp, entry, node) {
        hash_del(&entry->node);  // Remove from hash table
        kfree(entry);            // Free the allocated memory
    }
}


void do_full_scan(void){
	free_hashmap();
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
	struct page *page;
	for_each_node_state(nid,N_MEMORY){
		pgdat = NODE_DATA(nid);
		for (zoneid = 0; zoneid <= pgdat->kcompactd_highest_zoneidx; zoneid++) {
			zone = &pgdat->node_zones[zoneid];
			scan_zone(zone, &scan_data);
		}
	}
	nid = 0;
	scan_kernel_modules();
	scan_kernel_text();
	scan_hashmap_stats();
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
	free_hashmap();
    proc_remove(proc_entry);
    printk(KERN_INFO "m_scan unloaded\n");
}

module_init(gfn_module_init);
module_exit(gfn_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("m_scan module");


