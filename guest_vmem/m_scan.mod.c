#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x4c03a563, "random_kmalloc_seed" },
	{ 0xa63b4eed, "kmalloc_caches" },
	{ 0x59ffeca6, "kmalloc_trace" },
	{ 0xd67eae7c, "boot_cpu_data" },
	{ 0xa47553f, "tdx_kvm_hypercall" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0x8b9200fd, "lookup_address" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x4c9d28b0, "phys_base" },
	{ 0x3199fbeb, "mem_section" },
	{ 0x1e362ae8, "pcpu_hot" },
	{ 0x97651e6c, "vmemmap_base" },
	{ 0xbcb36fe4, "hugetlb_optimize_vmemmap_key" },
	{ 0x48d88a2c, "__SCT__preempt_schedule" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0x37a0cba, "kfree" },
	{ 0x9e3a3ffd, "proc_remove" },
	{ 0x9493fc86, "node_states" },
	{ 0x8810754a, "_find_first_bit" },
	{ 0x1e79f889, "node_data" },
	{ 0x53a1e8d9, "_find_next_bit" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xec0bd42d, "proc_create" },
	{ 0x122c3a7e, "_printk" },
	{ 0xf079b8f9, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "8AD5CC748495D8290FF589D");
