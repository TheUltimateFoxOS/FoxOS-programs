#include <stdio.h>

#include <stdint.h>
#include <assert.h>

#include <foxos/disk_access.h>

extern uint8_t nyan_mbr[];
extern uint64_t nyan_mbr_size;

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s <device>\n", argv[0]);
		return 1;
	}

	assert(nyan_mbr_size == 512);

	printf("Writing nyan mbr to %s... ", argv[1]);

	foxos_write_sector_raw(argv[1], 0, 1, (uint64_t) nyan_mbr);

	printf("Done.\n");
	return 0;
}