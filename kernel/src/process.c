#include <linux/kernel.h>	/* Для printk(), uintptr_t */
#include <linux/sched.h>	/* Для task_struct, mm_struct, vm_area_struct */
#include <linux/sched/mm.h>	/* Для mmput() */
#include <linux/slab.h>		/* Для kmalloc(), kfree() */
#include <linux/fs.h>		/* Для file_path() */

#include "process.h"

#define PATH_CHARS_MAX 256
#define MAP_ENTRIES_MAX 32

extern int founded_vmas_count;

struct map_entry *get_module_info(pid_t pid, char *name)
{
	struct pid *pid_struct;
	struct task_struct *task;
	struct mm_struct *mm;
	struct vm_area_struct *vma;
	struct map_entry *maps;
	int i = 0;

	maps = kmalloc(MAP_ENTRIES_MAX * sizeof(struct map_entry), GFP_KERNEL);
	if (!maps) {
		pr_err("[KernelHack]: failed kmalloc(...)");
		return NULL;
	}

	pid_struct = find_get_pid(pid);					if (!pid_struct) return NULL;
	task = get_pid_task(pid_struct, PIDTYPE_PID);	if (!task) return NULL;
	mm = get_task_mm(task);							if (!mm) return NULL;
	mmput(mm);

	for (vma = mm->mmap; vma; vma = vma->vm_next)
	{
		char buf[PATH_CHARS_MAX];
		char *path_nm = "";

		if (vma->vm_file)
		{
			path_nm = file_path(vma->vm_file, buf, PATH_CHARS_MAX - 1);
			printk("[KernelHack]:path_nm = %s", path_nm);
			printk("[KernelHack]:kbasenm = %s", kbasename(path_nm));
			//if (!strcmp(kbasename(path_nm), name))
			if (strstr(path_nm, name))
			{
				if (i == MAP_ENTRIES_MAX-1) break;
				maps[i].start	= vma->vm_start;
				maps[i].end		= vma->vm_end;
				maps[i].offset	= vma->vm_pgoff;
				maps[i].flags	= vma->vm_flags;
				i++;
			}
		}
	}
	founded_vmas_count = i;
	return maps;
}
