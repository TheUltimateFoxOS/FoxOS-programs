#include <test_disk.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <sys/open.h>
#include <sys/write.h>

struct raw_disk_dev_fs_command {
	uint8_t command;
	uint64_t sector;
	uint32_t sector_count;
	uint64_t buffer;
};

struct gpt_guid {
	uint32_t data1;
	uint16_t data2;
	uint16_t data3;
	uint64_t data4;
} __attribute__((packed));

struct gpt_header {
	char signature[8];
	uint8_t revision[4];
	uint32_t header_size;
	uint32_t crc32;
	uint32_t reserved;
	uint64_t current_lba;
	uint64_t backup_lba;
	uint64_t first_usable_lba;
	uint64_t last_usable_lba;
	struct gpt_guid guid;
	uint64_t partition_entries_startting_lba;
	uint32_t partition_entries_count;
	uint32_t partition_entries_size;
	uint32_t partition_entry_array_crc32;
} __attribute__((packed));

int disk(int argc, char* argv[], char* envp[]) {
    int fd = open("dev:disk_1");

	char buffer[512];
	struct gpt_header* header = (struct gpt_header*) buffer;

	struct raw_disk_dev_fs_command cmd = {
		.command = 0,
		.sector = 1,
		.sector_count = 1,
		.buffer = (uint64_t) header
	};

	write(fd, &cmd, sizeof(cmd), 0);

	if (memcmp(header->signature, "EFI PART", 8) == 0) {
		printf("EFI partition found\n");
	} else {
		printf("EFI partition not found\n");
	}

	for (int i = 0; i < 512; i++) {
		printf("%c", buffer[i]);
	}

    return 0;
}