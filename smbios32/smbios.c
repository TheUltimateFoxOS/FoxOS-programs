#include <smbios.h>

#include <string.h>

smbios_structure_handler_t smbios_structure_handlers[128] = { 0 };

smbios_structure_header_t* sm32_next(smbios_structure_header_t* structure) {
	char* x = (char*) structure;
	x += structure->length;

	size_t len = 0;
	
	// If value at ptr is 0, then offset by 2 and return,
	//  else, get the length of the region in memory until
	//  a null byte is encountered, add that to
	if (*x == 0) {
		x += 2; 
	} else do {
		len  = strlen(x);
		x += len + 1;
	} while(len > 0);

	return (smbios_structure_header_t*) x;
}

char* sm32_get_string(smbios_structure_header_t* structure, str_id id) {
	char* str = (char*) structure + structure->length;

	while (id - 1 != 0) {
		str += strlen(str) + 1;
		id--;
	}

	return str;
}