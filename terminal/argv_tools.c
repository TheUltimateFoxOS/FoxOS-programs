#include <argv_tools.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

char* read_env(char* in) {
	char tmp[512] = { 0 };
	int tmp_idx = 0;

	for (int i = 0; i < strlen(in); i++) {
		if (in[i] == '$') {
			char env_var[256] = { 0 };
			int env_var_idx = 0;

			i++;

			while ((in[i] >= 'a' && in[i] <= 'z' || in[i] >= 'A' && in[i] <= 'Z' || in[i] >= '0' && in[i] <= '9' || in[i] == '_') && in[i] != 0) {
				env_var[env_var_idx] = in[i];
				env_var_idx++;
				i++;
			}

			// printf("env_var: %s\n", env_var);
			char* env_var_value = getenv(env_var);
			// printf("env_var_value: %s\n", env_var_value);

			if (env_var_value != NULL) {
				strcat(tmp, env_var_value);
				tmp_idx += strlen(env_var_value);
			}

			i--;
		} else {
			tmp[tmp_idx] = in[i];
			tmp_idx++;
		}
	}

	char* out = malloc(strlen(tmp) + 1);
	memset(out, 0, strlen(tmp) + 1);
	memcpy(out, tmp, strlen(tmp) + 1);
	return out;
}

char** argv_env_process(char** in) {
	int curr_arg = 0;
	while (in[curr_arg] != NULL) {
		in[curr_arg] = read_env(in[curr_arg]);
		curr_arg++;
	}

	return in;
}

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

void free_argv(char** argv) {
	for (int i = 0; argv[i] != NULL; i++) {
		free(argv[i]);
	}
	free(argv);
}