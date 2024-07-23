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
	{ 0x88db9f48, "__check_object_size" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x5c3c7387, "kstrtoull" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x8892a383, "single_open" },
	{ 0xba8fbd64, "_raw_spin_lock" },
	{ 0x20b8e413, "gfn_to_memslot" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0xb5b54b34, "_raw_spin_unlock" },
	{ 0xf1d94c00, "seq_printf" },
	{ 0x4c9d28b0, "phys_base" },
	{ 0xf9f6b762, "proc_remove" },
	{ 0xe5b2f88b, "seq_read" },
	{ 0xba5251f9, "seq_lseek" },
	{ 0x859ee6d8, "single_release" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0xe007de41, "kallsyms_lookup_name" },
	{ 0x6cdeed8e, "proc_create" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x9ffbb7a5, "module_layout" },
};

MODULE_INFO(depends, "kvm");


MODULE_INFO(srcversion, "CCE172C2AB5D92CB6B315D1");
