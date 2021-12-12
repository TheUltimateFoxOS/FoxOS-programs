#include <argv_tools.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

char** argv_split(char* str) {
	int len = strlen(str);

	int argc = 1;
	bool quote_open = false;
	bool double_quote_open = false;
	bool special_char_next = false;

	for (int i = 0; i < len; i++) {
		if(str[i] == ' ') {
			if (!quote_open && !double_quote_open) {
				argc++;
			}
		} else if (str[i] == '\\') {
			special_char_next = true;
		} else if (str[i] == '\"') {
			if (special_char_next || double_quote_open) {
				special_char_next = false;
			} else {
				quote_open = !quote_open;
			}
		} else if (str[i] == '\'') {
			if (special_char_next || quote_open) {
				special_char_next = false;
			} else {
				double_quote_open = !double_quote_open;
			}
		}
	}

	char** argv = malloc(sizeof(char*) * (argc + 1));

	argc = 1;
	argv[0] = &str[0];

	quote_open = false;
	double_quote_open = false;
	special_char_next = false;

	for (int i = 0; i < len; i++) {
		if(str[i] == ' ') {
			if (!quote_open && !double_quote_open) {
				argv[argc] = &str[i + 1];
				str[i] = 0;
				argc++;
			}
		} else if (str[i] == '\\') {
			special_char_next = true;
		} else if (str[i] == '\"') {
			if (special_char_next || double_quote_open) {
				special_char_next = false;
			} else {
				quote_open = !quote_open;
			}
		} else if (str[i] == '\'') {
			if (special_char_next || quote_open) {
				special_char_next = false;
			} else {
				double_quote_open = !double_quote_open;
			}
		}
	}

	argv[argc] = NULL;

	return argv;
}
