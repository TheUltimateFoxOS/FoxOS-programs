#pragma once

#include <stdint.h>

typedef struct smbios_entry_t {
	char entry_point_signature[4];
	uint8_t checksum;
	uint8_t length;
	uint8_t major_version;
	uint8_t minor_version;
	uint16_t max_structure_size;
	uint8_t entry_point_revision;
	char formatted_area[5];
	char entry_point_string[5];
	uint8_t checksum_2;
	uint16_t table_length;
	uint32_t table_address;
	uint16_t number_of_structures;
	uint8_t bcd_revision;
} __attribute__((packed)) smbios_entry_t;

typedef struct smbios_structure_header_t {
	uint8_t type;
	uint8_t length;
	uint16_t handle;
} __attribute__((packed)) smbios_structure_header_t;

typedef uint8_t str_id;

smbios_structure_header_t* sm32_next(smbios_structure_header_t* structure);
char* sm32_get_string(smbios_structure_header_t* structure, str_id id);

typedef struct smbios_structure_handler_t {
	char* name;
	void (*handler)(smbios_structure_header_t*);
} smbios_structure_handler_t;

extern smbios_structure_handler_t smbios_structure_handlers[128];