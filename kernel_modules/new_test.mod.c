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
	{ 0xf9f6b762, "proc_remove" },
	{ 0xba8fbd64, "_raw_spin_lock" },
	{ 0x64a351ae, "__tracepoint_mmap_lock_start_locking" },
	{ 0x668b19a1, "down_read" },
	{ 0x39ce9680, "__tracepoint_mmap_lock_acquire_returned" },
	{ 0x34f84e64, "follow_pte" },
	{ 0xf1d94c00, "seq_printf" },
	{ 0xb5b54b34, "_raw_spin_unlock" },
	{ 0xfea99ed9, "__tracepoint_mmap_lock_released" },
	{ 0x53b954a2, "up_read" },
	{ 0x1e698e2a, "gfn_to_hva" },
	{ 0xf7ff86c5, "gfn_to_pfn" },
	{ 0x4e3fd1b4, "kvm_release_pfn_clean" },
	{ 0xb8f3bc88, "__mmap_lock_do_trace_released" },
	{ 0xef43ff65, "__mmap_lock_do_trace_acquire_returned" },
	{ 0xd1808488, "__mmap_lock_do_trace_start_locking" },
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


MODULE_INFO(srcversion, "9930C98B720CDA0D0072DAF");
