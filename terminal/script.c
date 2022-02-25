#include <script.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool command_received(char* command, bool* should_break);

char* process_line(char* command) {
	int i;
	int j;
	int command_length = strlen(command);

	char* output_1 = command;
	bool text_found = false;

	for (i = 0, j = 0; i < command_length; i++, j++) {
		if (command[i] == ' ' && !text_found) {
			j--;
		} else {
			output_1[j] = command[i];
			text_found = true;
		}
	}
	output_1[j] = 0;

	command_length = strlen(output_1);
	if (command_length == 0) {
		return output_1;
	}

	char* output_2 = output_1;
	for (i = 0; i < command_length; i++) {
		if (output_1[i] == '#') {
			output_2[i] = 0;
			break;
		} else {
			output_2[i] = output_1[i];
		}
	}

	command_length = strlen(output_1);
	if (command_length == 0) {
		return output_2;
	}

	char* output_3 = output_2;
	for (i = command_length - 1; i > 0; i--) {
		if (output_3[i] == ' ') {
			output_3[i] = 0;
		} else {
			break;
		}
	}

	return output_3;
}

void run_script(char* path, int max_buffer_size) {
	char resolved_path[256] = { 0 };
	bool canresolve = resolve(path, resolved_path);
	if (!canresolve) {
		printf("Error: Could not resolve path: '%s'\n", path);
		abort();
	}
	// printf("Running script: '%s'\n", path);

	FILE* file = fopen(resolved_path, "r");
	if (file == NULL) {
		printf("Error: Failed to open script file: '%s'\n", path);
		abort();
	}

	char* script_data = malloc(file->size + 1);
	memset(script_data, 0, file->size + 1);
	fread(script_data, file->size, 1, file);

	int line_number = 0;
	char current_line[256] = { 0 };
	int script_idx = 0;
	int idx = 0;

	do {
		idx = 0;
		while (true) {
			if (script_idx >= file->size) {
				break;
			}

			if (script_data[script_idx] == '\n') {
				current_line[idx] = '\0';
				script_idx++;
				line_number++;
				break;
			} else {
				current_line[idx] = script_data[script_idx];
			}

			script_idx++;
			idx++;
		}

		if (current_line[0] == '#') {
			continue;
		}

		if (current_line[0] == '\0') {
			continue;
		}

		//printf("In line: '%s'\n", current_line);
		char* processed_line = process_line(current_line);
		if (processed_line[0] == '\0') {
			continue;
		}
		//printf("Out line: '%s'\n", processed_line);

		char command[256] = { 0 };
		memcpy(command, processed_line, strlen(processed_line) + 1);

		bool should_break;
		bool command_found = command_received(command, &should_break); //This should block while command is running.
		if (!command_found) {
			printf("Error: Command not found: '%s', line %d\n", command, line_number);
			abort();
		}
		memset(current_line, 0, 256);

		if (should_break == true) {
			break;
		}
	} while (script_idx < file->size);

	fclose(file); // we still use the file object in the loop so we can only close it here.
	free(script_data);
	return;
}