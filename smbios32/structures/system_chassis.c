#include <smbios_structures.h>

typedef struct system_chassis_t {
	str_id manufacturer;
	uint8_t type;
	str_id version;
	str_id serial_number;
	str_id assert_tag;
} __attribute__((packed)) system_chassis_t;

void system_chassis_handler_func(smbios_structure_header_t* header) {
	system_chassis_t* system_chassis = (system_chassis_t*) ((uint64_t) header + sizeof(smbios_structure_header_t));
	printf("> manufacturer: %s\n", sm32_get_string(header, system_chassis->manufacturer));
	printf("> type: %d\n", system_chassis->type);
	printf("> version: %s\n", sm32_get_string(header, system_chassis->version));
	printf("> serial_number: %s\n", sm32_get_string(header, system_chassis->serial_number));
	printf("> assert_tag: %s\n", sm32_get_string(header, system_chassis->assert_tag));
}

smbios_structure_handler_t system_chassis_handler = {
	.name = "system_chassis",
	.handler = system_chassis_handler_func
};