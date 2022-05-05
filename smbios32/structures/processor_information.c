#include <smbios_structures.h>

typedef struct processor_information_t {
	str_id socket_designation;
	uint8_t processor_type;
	uint8_t processor_family;
	str_id manufacturer;
	uint64_t processor_id;
	str_id processor_version;
	uint8_t voltage;
	uint16_t external_clock;
	uint16_t max_speed;
	uint16_t current_speed;
	uint8_t status;
	uint8_t processor_upgrade;
} __attribute__((packed)) processor_information_t;

void processor_information_handler_func(smbios_structure_header_t* header) {
	processor_information_t* processor_information = (processor_information_t*) ((uint64_t) header + sizeof(smbios_structure_header_t));
	printf("> socket_designation: %s\n", sm32_get_string(header, processor_information->socket_designation));
	printf("> processor_type: %d\n", processor_information->processor_type);
	printf("> processor_family: %d\n", processor_information->processor_family);
	printf("> manufacturer: %s\n", sm32_get_string(header, processor_information->manufacturer));
	printf("> processor_id: %d\n", processor_information->processor_id);
	printf("> processor_version: %s\n", sm32_get_string(header, processor_information->processor_version));
	printf("> voltage: %d\n", processor_information->voltage);
	printf("> external_clock: %d\n", processor_information->external_clock);
	printf("> max_speed: %d\n", processor_information->max_speed);
	printf("> current_speed: %d\n", processor_information->current_speed);
	printf("> status: %d\n", processor_information->status);
	printf("> processor_upgrade: %d\n", processor_information->processor_upgrade);
}

smbios_structure_handler_t processor_information_handler = {
	.name = "processor_information",
	.handler = processor_information_handler_func
};