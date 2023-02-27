#include <scanner/fexec_scanner.h>
#include <file_scanner.h>

#include <stdio.h>
#include <string.h>

bool is_fexec(void* buffer, size_t size) {
	fexec_header_t* header = (fexec_header_t*) buffer;
	return header->magic == FEXEC_MAGIC;
}

void get_fexec_information(void* buffer, size_t size, char* out, size_t out_size) {
	fexec_header_t* header = (fexec_header_t*) buffer;

	if (header->version != 1) {
		strcat(out, "Unknown version");
		return;
	}
	strcat(out, "Version 1 executable");
}

file_scanner_t fexec_scanner = {
	.is_file = is_fexec,
	.get_information = get_fexec_information,
	.name = "Fexec File",
};