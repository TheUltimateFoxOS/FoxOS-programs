#include <stdio.h>
#include <stdlib.h>

#include <smbios.h>
#include <smbios_structures.h>

int main(int argc, char *argv[]) {
	char* smbios32_file = "dev:smbios32";

	FILE* f = fopen(smbios32_file, "r");
	if (f == NULL) {
		printf("Error: could not open %s\n", smbios32_file);
		return 1;
	}

	void* smbios32_data = malloc(f->size);
	fread(smbios32_data, f->size, 1, f);
	fclose(f);

	smbios_entry_t* smbios = (smbios_entry_t*) smbios32_data;
	
	printf("SMBIOS32 entry struct:\n", smbios);
	printf("> entry_point_signature: %c%c%c%c\n", smbios->entry_point_signature[0], smbios->entry_point_signature[1], smbios->entry_point_signature[2], smbios->entry_point_signature[3]);
	printf("> checksum: 0x%x\n", smbios->checksum);
	printf("> length: 0x%x\n", smbios->length);
	printf("> major_version: 0x%x\n", smbios->major_version);
	printf("> minor_version: 0x%x\n", smbios->minor_version);
	printf("> max_structure_size: 0x%x\n", smbios->max_structure_size);
	printf("> entry_point_revision: 0x%x\n", smbios->entry_point_revision);
	printf("> entry_point_string: %c%c%c%c%c\n", smbios->entry_point_string[0], smbios->entry_point_string[1], smbios->entry_point_string[2], smbios->entry_point_string[3], smbios->entry_point_string[4]);
	printf("> checksum_2: 0x%x\n", smbios->checksum_2);
	printf("> table_length: 0x%x\n", smbios->table_length);
	printf("> table_address: 0x%x\n", smbios->table_address);
	printf("> number_of_structures: 0x%x\n", smbios->number_of_structures);
	printf("> bcd_revision: 0x%x\n\n", smbios->bcd_revision);

	smbios_structure_handlers[0] = bios_information_handler;
	smbios_structure_handlers[1] = system_information_handler;
	smbios_structure_handlers[3] = system_chassis_handler;
	smbios_structure_handlers[4] = processor_information_handler;

	smbios_structure_header_t* structure = (smbios_structure_header_t*) ((uint64_t) smbios + sizeof(smbios_entry_t));

	for (uint32_t i = 0; i < smbios->number_of_structures; i++) {
		if (smbios_structure_handlers[structure->type].name != NULL) {
			printf("Structure %d: %s\n", i, smbios_structure_handlers[structure->type].name);
			smbios_structure_handlers[structure->type].handler(structure);
		} else {
			printf("Structure %d: unknown (type %d)\n", i, structure->type);
		}

		structure = sm32_next(structure);
	}

	free(smbios32_data);
	return 0;
}