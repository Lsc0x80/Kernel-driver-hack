#pragma pack(push, 1)
struct map_entry {
	uintptr_t start;
	uintptr_t end;
	unsigned long offset;
	unsigned long flags;
};
#pragma pack(pop)

struct map_entry *get_module_info(pid_t pid, char *name);
