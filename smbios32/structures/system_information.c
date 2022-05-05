#include <smbios_structures.h>

typedef struct system_info_t {
	str_id manufacturer;
	str_id product_name;
	str_id version;
	str_id serial_number;
} __attribute__((packed)) system_info_t;

void system_information_handler_func(smbios_structure_header_t* header) {
	system_info_t* system_information = (system_info_t*) ((uint64_t) header + sizeof(smbios_structure_header_t));
	printf("> manufacturer: %s\n", sm32_get_string(header, system_information->manufacturer));
	printf("> product_name: %s\n", sm32_get_string(header, system_information->product_name));
	printf("> version: %s\n", sm32_get_string(header, system_information->version));
	printf("> serial_number: %s\n", sm32_get_string(header, system_information->serial_number));
}

smbios_structure_handler_t system_information_handler = {
	.name = "system_information",
	.handler = system_information_handler_func
};