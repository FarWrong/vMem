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
	{ 0x6c31ab71, "proc_remove" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x5c3c7387, "kstrtoull" },
	{ 0x205202f9, "vm_list" },
	{ 0x547f5b96, "gfn_to_hva" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0x4b88301c, "get_user_pages_remote" },
	{ 0x97651e6c, "vmemmap_base" },
	{ 0xbcb36fe4, "hugetlb_optimize_vmemmap_key" },
	{ 0x587f22d7, "devmap_managed_key" },
	{ 0x27246bce, "__folio_put" },
	{ 0x9580745, "__put_devmap_managed_page_refs" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xcdf68da8, "proc_create" },
	{ 0x122c3a7e, "_printk" },
	{ 0x33b7d108, "module_layout" },
};

MODULE_INFO(depends, "kvm");


MODULE_INFO(srcversion, "E4418FDFC1DA4802115CC81");
