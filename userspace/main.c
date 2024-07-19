#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

#define DEVICE_NAME "/dev/rAnd0m"

enum OPS {
	GET_MAPS    		= 6001,
	GET_MAPS_PRESENCE	= 6002,
	READ_MEM    		= 6003,
	WRITE_MEM   		= 6004,
};

#pragma pack(push, 1)
struct map_entry {
	uintptr_t start;
	uintptr_t end;
	unsigned long offset;
	unsigned long flags;
};
#pragma pack(pop)

struct maps_ {
	struct map_entry *founded;
	int founded_count;

	// pass to kernel
	pid_t pid;
	char *name_to_find;
};

struct module_presence {
	pid_t pid;
	char *module_name;

	int presence;
};

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("usage: %s [module name]\n", argv[0]);
		return 1;
	}
	int fd = open(DEVICE_NAME, O_RDWR);
	if (fd == -1) {
		printf("Open driver failed!\n");
		return 1;
	}

	// Reading /proc/self/maps from userspace
	printf("Memory mapping from /proc/self/maps:\n");
	int maps_founded_count = 0;
	FILE *fp;
	char buf[1024];
	fp = fopen("/proc/self/maps", "r");
	if (fp == NULL) {
		perror("fopen");
		return 1;
	}
	while (fgets(buf, sizeof(buf), fp)) {
		if (strstr(buf, argv[1 ])) {
			maps_founded_count++;
			printf("%s", buf);
		}
	}
	fclose(fp);
	printf("Founded %d regions\n", maps_founded_count);

	// Reading maps from kernel module
	printf("\nMemory mapping from kernel module:\n");
	struct maps_ maps;
	unsigned long flags;

	maps.pid = getpid();
	maps.name_to_find = argv[1];
	maps.founded = malloc(32*sizeof(struct map_entry));

	ioctl(fd, GET_MAPS, &maps);

	for (int i = 0; i < maps.founded_count; i++) {
		flags = maps.founded[i].flags;
		// Range
		printf("%lx-%lx ", maps.founded[i].start, maps.founded[i].end);
		// Flags
		if (flags&PROT_READ)  printf("r"); else printf("-");
		if (flags&PROT_WRITE) printf("w"); else printf("-");
		if (flags&PROT_EXEC)  printf("x"); else printf("-");
		printf("p");
		// Offset
		printf(" %08lx", maps.founded[i].offset * 0x1000);
		printf("\n");
	}
	printf("Founded %d regions\n", maps.founded_count);
	free(maps.founded);

	// Check for module presence
	struct module_presence mod_p;
	mod_p.pid = getpid();
	mod_p.module_name = argv[1];

	ioctl(fd, GET_MAPS_PRESENCE, &mod_p);
	printf("\nPresence: %d\n", mod_p.presence);

	return 0;
}
