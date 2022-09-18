#include <file_scanner.h>

#include <stdio.h>
#include <stdint.h>
#include <foxdb.h>

bool is_fdb(void* buffer, size_t size) {
	return ((foxdb_file_header_t*) buffer)->magic == FOXDB_MAGIC;
}

void get_fdb_information(void* buffer, size_t size, char* out, size_t out_size) {
	int num_keys = 0;

	foxdb_file_header_t* header = (foxdb_file_header_t*) buffer;

	uint64_t curr_offset = sizeof(foxdb_file_header_t);
	
	while (curr_offset < header->size) {
		foxdb_entry_t* curr = (foxdb_entry_t*) &((uint8_t*) header)[curr_offset];
		num_keys++;
		curr_offset += curr->size;
	}

	snprintf(out, out_size, "Contains %d keys.", num_keys);
}

file_scanner_t fdb_scanner = {
	.is_file = is_fdb,
	.get_information = get_fdb_information,
	.name = "FoxOS database",
};