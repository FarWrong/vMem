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



static const char ____versions[]
__used __section("__versions") =
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x1c\x00\x00\x00\x63\xa5\x03\x4c"
	"random_kmalloc_seed\0"
	"\x18\x00\x00\x00\x10\x03\x98\x24"
	"kmalloc_caches\0\0"
	"\x18\x00\x00\x00\xeb\x9d\x19\x1d"
	"kmalloc_trace\0\0\0"
	"\x18\x00\x00\x00\x4a\xab\x6c\xe1"
	"boot_cpu_data\0\0\0"
	"\x1c\x00\x00\x00\x3f\x55\x47\x0a"
	"tdx_kvm_hypercall\0\0\0"
	"\x14\x00\x00\x00\xb0\x28\x9d\x4c"
	"phys_base\0\0\0"
	"\x18\x00\x00\x00\xfd\x00\x92\x8b"
	"lookup_address\0\0"
	"\x1c\x00\x00\x00\xcb\xf6\xfd\xf0"
	"__stack_chk_fail\0\0\0\0"
	"\x14\x00\x00\x00\xeb\xfb\x99\x31"
	"mem_section\0"
	"\x14\x00\x00\x00\xcf\xe2\xb6\x02"
	"pcpu_hot\0\0\0\0"
	"\x18\x00\x00\x00\x6c\x1e\x65\x97"
	"vmemmap_base\0\0\0\0"
	"\x28\x00\x00\x00\xe4\x6f\xb3\xbc"
	"hugetlb_optimize_vmemmap_key\0\0\0\0"
	"\x20\x00\x00\x00\x2c\x8a\xd8\x48"
	"__SCT__preempt_schedule\0"
	"\x28\x00\x00\x00\xb3\x1c\xa2\x87"
	"__ubsan_handle_out_of_bounds\0\0\0\0"
	"\x2c\x00\x00\x00\xc6\xfa\xb1\x54"
	"__ubsan_handle_load_invalid_value\0\0\0"
	"\x10\x00\x00\x00\xba\x0c\x7a\x03"
	"kfree\0\0\0"
	"\x14\x00\x00\x00\x52\x4d\x35\x6a"
	"proc_remove\0"
	"\x14\x00\x00\x00\x86\xfc\x93\x94"
	"node_states\0"
	"\x18\x00\x00\x00\x4a\x75\x10\x88"
	"_find_first_bit\0"
	"\x14\x00\x00\x00\x27\x99\x51\xa9"
	"node_data\0\0\0"
	"\x18\x00\x00\x00\xd9\xe8\xa1\x53"
	"_find_next_bit\0\0"
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x14\x00\x00\x00\xc1\xb2\x4a\x4f"
	"proc_create\0"
	"\x10\x00\x00\x00\x7e\x3a\x2c\x12"
	"_printk\0"
	"\x18\x00\x00\x00\xd7\xd3\x75\x6d"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "30659C8A115B28F58FFDA29");
