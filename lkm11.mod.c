#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
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
__used
__attribute__((section("__versions"))) = {
	{ 0x516e49f9, "module_layout" },
	{ 0x3e1e98e2, "class_unregister" },
	{ 0xaf87d05c, "device_destroy" },
	{ 0xb4f66f3b, "kthread_stop" },
	{ 0x2b68bd2f, "del_timer" },
	{ 0xfe990052, "gpio_free" },
	{ 0xe346f67a, "__mutex_init" },
	{ 0xe48cc71f, "wake_up_process" },
	{ 0x9a67bc9f, "kthread_create_on_node" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0x3df0823f, "class_destroy" },
	{ 0x7c1318e3, "device_create" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xebdab104, "__class_create" },
	{ 0xa140bc36, "__register_chrdev" },
	{ 0xec68e223, "dequeue_signal" },
	{ 0x952664c5, "do_exit" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0x6df1aaf1, "kernel_sigaction" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x67ea780, "mutex_unlock" },
	{ 0xc271c3be, "mutex_lock" },
	{ 0x5f754e5a, "memset" },
	{ 0x1e047854, "warn_slowpath_fmt" },
	{ 0x28cc25db, "arm_copy_from_user" },
	{ 0x4e8b35ac, "gpiod_direction_output_raw" },
	{ 0x3555ff59, "gpiod_get_raw_value" },
	{ 0x3e8ff20c, "gpiod_direction_input" },
	{ 0xf9a482f9, "msleep" },
	{ 0xddc1fca4, "gpiod_set_raw_value" },
	{ 0x395c5ae9, "gpio_to_desc" },
	{ 0xc38c83b8, "mod_timer" },
	{ 0x526c3a6c, "jiffies" },
	{ 0xe4d8280e, "send_sig" },
	{ 0x2e5810c6, "__aeabi_unwind_cpp_pr1" },
	{ 0x7c32d0f0, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

