#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define arg(name) bool name = false; for (int i = 1; i < argc; i++) { if (strcmp(argv[i], "--"#name) == 0) { name = true; break; } }
#define arg_str(name) char* name = NULL; for (int i = 1; i < argc; i++) { if (strcmp(argv[i], "--"#name) == 0) { name = argv[i + 1]; break; } }

char* remove_quotes(char* str) {
	if (str[0] == '"') {
		str[strlen(str) - 1] = '\0';
		return str + 1;
	} else {
		return str;
	}
}

enum compare_type {
	EQUAL = 1,
	LESS,
	MORE
};

int parse_compare_type(char* str) {
	if (strcmp(str, "==") == 0) {
		return EQUAL;
	} else if (strcmp(str, "<") == 0) {
		return LESS;
	} else if (strcmp(str, ">") == 0) {
		return MORE;
	} else {
		return 0;
	}
}

int parse_number(char* input) {
	bool hex = false;
	bool bin = false;
	bool dec = false;

	int start_offset = 0;

	if (input[0] == '0') {
		if (input[1] == 'x') {
			hex = true;
			start_offset = 2;
		} else if (input[1] == 'b') {
			bin = true;
			start_offset = 2;
		} else {
			dec = true;
		}
	} else {
		dec = true;
	}

	int number_system_base = 0;

	if (hex) {
		number_system_base = 16;
	} else if (bin) {
		number_system_base = 2;
	} else {
		number_system_base = 10;
	}

	int _number = 0;
	int idx = start_offset;

	while (input[idx] != '\0') {
		if (input[idx] >= '0' && input[idx] <= '9') {
			_number = _number * number_system_base + (input[idx] - '0');
		} else if (input[idx] >= 'a' && input[idx] <= 'f') {
			_number = _number * number_system_base + (input[idx] - 'a' + 10);
		} else if (input[idx] >= 'A' && input[idx] <= 'F') {
			_number = _number * number_system_base + (input[idx] - 'A' + 10);
		} else {
			break;
		}

		idx++;
	}

	return _number;
}

char* strip_string(char* in) {
	if (in[strlen(in) - 1] == ' ') {
		in[strlen(in) - 1] = '\0';
	}

	if (in[0] == ' ') {
		return in + 1;
	} else {
		return in;
	}	
}

int main(int argc, char *argv[]) {
	arg_str(val1);
	arg_str(val2);
	arg_str(cmp);
	arg_str(cmd);

	arg(not);

	if (val1 == NULL || val2 == NULL || cmp == NULL) {
		printf("Usage: %s [--val1 <str>] [--val2 <str>] [--cmp <==/</>>] [--not] [--cmd <command>]\n", argv[0]);
		return 1;
	}

	int cmp_p = parse_compare_type(cmp);
	if (cmp_p == 0) {
		printf("Invalid compare type\n");
		return 1;
	}

	if (cmd) {
		cmd = remove_quotes(cmd);
	} else {
		cmd = "echo command not set";
	}

	val1 = strip_string(val1);
	val2 = strip_string(val2);

#ifdef IF_DEBUG
	printf("Compare summary:\n");
	printf("  > val1: '%s'\n", val1);
	printf("  > val2: '%s'\n", val2);
	printf("  > cmp: %s\n", cmp_p);
	printf("  > cmd: %s\n", cmd);
	printf("  > not: %s\n", not ? "true" : "false");
#endif

	switch (cmp_p) {
		case EQUAL:
			if (strcmp(val1, val2) == 0) {
				if (!not) {
					system(cmd);
				}
			} else {
				if (not) {
					system(cmd);
				}
			}
			break;
		
		case LESS:
			if (parse_number(val1) < parse_number(val2)) {
				if (!not) {
					system(cmd);
				}
			} else {
				if (not) {
					system(cmd);
				}
			}
			break;
		
		case MORE:
			if (parse_number(val1) > parse_number(val2)) {
				if (!not) {
					system(cmd);
				}
			} else {
				if (not) {
					system(cmd);
				}
			}
			break;
		
		default:
			printf("This should never happen\n");
			return 1;
	}

	return 0;
}