#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <buildin/number_parser.h>
#include <buildin/arg.h>

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
			{
				int num1;
				__libc_parse_number(val1, &num1);
				int num2;
				__libc_parse_number(val2, &num2);
				if (num1 < num2) {
					if (!not) {
						system(cmd);
					}
				} else {
					if (not) {
						system(cmd);
					}
				}
			}
			break;
		
		case MORE:
			{
				int num1;
				__libc_parse_number(val1, &num1);
				int num2;
				__libc_parse_number(val2, &num2);
				if (num1 > num2) {
					if (!not) {
						system(cmd);
					}
				} else {
					if (not) {
						system(cmd);
					}
				}
			}
			break;
		
		default:
			printf("This should never happen\n");
			return 1;
	}

	return 0;
}