#include <linux/kernel.h>		/* Для работы с ядром */
#include <linux/module.h>		/* Нужно для каждого модуля ядра */
#include <linux/miscdevice.h>	/* Для struct miscdevice, misc_register() */
#include <linux/uaccess.h>		/* Для copy_from_user, copy_to_user */
#include <linux/slab.h>			/* Для kmalloc(), kfree() */

#include "process.h"

#define DEVICE_NAME "rAnd0m"

int founded_vmas_count;

enum OPS {
	GET_MAPS	= 6001,
	READ_MEM	= 6002,
	WRITE_MEM	= 6003,
};

struct maps_ {
	// output
	struct map_entry *founded;
	int founded_count;

	// input
	pid_t pid;
	char *name_to_find;
};

static int dispatch_open(struct inode *node, struct file *file)
{
	return 0;
}

static int dispatch_close(struct inode *node, struct file *file)
{
	return 0;
}

static ssize_t dispatch_read(struct file *filePointer, char __user *buffer,
						size_t buffer_length, loff_t *offset)
{
	char s[26] = "You need use ioctl in C!\n";
	int len = sizeof(s);
	ssize_t ret = len;

	if (*offset >= len || copy_to_user(buffer, s, len)) {
		pr_info("[KernelHack]: copy_to_user failed\n");
		ret = 0;
	} else {
		pr_info("[KernelHack]: chardev read %s\n", filePointer->f_path.dentry->d_name.name); 
		*offset += len;
	}

	return ret;
}

static long dispatch_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	static char name[0x100] = {0};
	struct maps_ maps;
	struct map_entry *founded_vmas;

	switch (cmd)
	{
		case GET_MAPS:
		{
			pr_info("[KernelHack]: Getting maps");
			if (copy_from_user(&maps, (void __user *)arg, sizeof(maps)) != 0 || copy_from_user(name, (void __user *)maps.name_to_find, sizeof(name) - 1) != 0)
			{
				pr_info("[KernelHack]: error when copying from user");
				return -1;
			}
			founded_vmas = get_module_info(maps.pid, name);
			maps.founded_count = founded_vmas_count;

			if (!founded_vmas) {
				pr_info("[KernelHack]: get_module_info returned null pointer");
				return -1;
			}
			if (copy_to_user((void __user *)arg, &maps, sizeof(maps)) != 0 || copy_to_user((void __user *)maps.founded, founded_vmas, 32 * sizeof(struct map_entry)) != 0 )
			{
				pr_info("[KernelHack]: error when copying to user");
				kfree(founded_vmas);
				return -1;
			}
			kfree(founded_vmas);
			pr_info("[KernelHack]: Getting maps done");
			break;
		}
		default:
			break;
	}
	return 0;
}

static struct file_operations dispatch_functions = {
	.owner			= THIS_MODULE,
	.open			= dispatch_open,
	.release		= dispatch_close,
	.unlocked_ioctl = dispatch_ioctl,
	.read			= dispatch_read,
};

static struct miscdevice misc = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= DEVICE_NAME,
	.fops	= &dispatch_functions,
};

static int __init driver_entry(void)
{
	printk("[KernelHack]: [+] driver_entry");
	return misc_register(&misc);
}

static void __exit driver_unload(void)
{
	printk("[KernelHack]: [+] driver_unload");
	misc_deregister(&misc);
}

module_init(driver_entry);
module_exit(driver_unload);

MODULE_DESCRIPTION("Linux Kernel <-> Userspace");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lsc0x");
