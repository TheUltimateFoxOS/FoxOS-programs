#include <disk_access.h>

#include <sys/write.h>
#include <sys/open.h>
#include <sys/close.h>
#include <stdio.h>
#include <stdlib.h>
void read_sector_raw(char* device, uint64_t sector, uint32_t sector_count, uint64_t buffer) {
	struct raw_disk_dev_fs_command command;
	command.command = 0;
	command.sector = sector;
	command.sector_count = sector_count;
	command.buffer = buffer;
	
	int fd = open(device);
	if (fd < 0) {
		printf("open(%s) failed\n", device);
		abort();
	}
	
	write(fd, &command, sizeof(command), 0);
	
	close(fd);
}

void write_sector_raw(char* device, uint64_t sector, uint32_t sector_count, uint64_t buffer) {
	struct raw_disk_dev_fs_command command;
	command.command = 1;
	command.sector = sector;
	command.sector_count = sector_count;
	command.buffer = buffer;
	
	int fd = open(device);
	if (fd < 0) {
		printf("open(%s) failed\n", device);
		abort();
	}
	
	write(fd, &command, sizeof(command), 0);
	
	close(fd);
}
