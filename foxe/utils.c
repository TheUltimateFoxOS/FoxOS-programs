#include <utils.h>

#include <stddef.h>
#include <string.h>

char* get_file_extension(const char* filename) {
	char* chr_ptr = strchr(filename, '.');
	if (chr_ptr == NULL) {
		return "";
	}
	return ++chr_ptr;
}