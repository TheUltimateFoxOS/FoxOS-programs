#pragma once

#include <stdint.h>

struct raw_disk_dev_fs_command {
	uint8_t command;
	uint64_t sector;
	uint32_t sector_count;
	uint64_t buffer;
};

void read_sector_raw(char* device, uint64_t sector, uint32_t sector_count, uint64_t buffer);
void write_sector_raw(char* device, uint64_t sector, uint32_t sector_count, uint64_t buffer);