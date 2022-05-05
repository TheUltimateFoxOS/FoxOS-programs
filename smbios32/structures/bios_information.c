#include <smbios_structures.h>

typedef struct bios_information_t {
	str_id vendor;
	str_id version;
	uint16_t start_segment;
	str_id release_date;
	uint8_t rom_size;
	uint64_t characteristics;
} __attribute__((packed)) bios_information_t;

void bios_information_handler_func(smbios_structure_header_t* header) {
	bios_information_t* bios_information = (bios_information_t*) ((uint64_t) header + sizeof(smbios_structure_header_t));
	printf("> vendor: %s\n", sm32_get_string(header, bios_information->vendor));
	printf("> version: %s\n", sm32_get_string(header, bios_information->version));
	printf("> start_segment: %d\n", bios_information->start_segment);
	printf("> release_date: %s\n", sm32_get_string(header, bios_information->release_date));
	printf("> rom_size: %d\n", bios_information->rom_size);
	printf("> characteristics: %d\n", bios_information->characteristics);
}

smbios_structure_handler_t bios_information_handler = {
	.name = "bios_information",
	.handler = bios_information_handler_func
};