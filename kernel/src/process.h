#pragma pack(push, 1)
struct map_entry {
	uintptr_t start;
	uintptr_t end;
	unsigned long offset;
	unsigned long flags;
};
#pragma pack(pop)

/*
 * Get array of founded modules.
 *
 *  Possible returns:
 * NULL - failed allocate memory or failed when getting mm_struct
 * struct map_entry[] - successfuly inited and iterated through vm_area_struct
 */
struct map_entry *get_module_info(pid_t pid, char *name);

/*
 * Get if memory map of process contains `name`
 *
 *  Possible returns:
 * -1 - failed when init.
 * 0  - memory map not contain `name`
 * 1  - memory map contain `name`
 */
int get_has_module(pid_t pid, char *name);
