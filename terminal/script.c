#include <script.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool command_received(char* command);

void run_script(char* path) {
	char resolved_path[256] = { 0 };
	bool canresolve = resolve(path, resolved_path);
	if (!canresolve) {
		printf("Could not resolve path: '%s'\n", path);
		abort();
	}
	// printf("Running script: '%s'\n", path);

	FILE* file = fopen(resolved_path, "r");
	if (file == NULL) {
		printf("Failed to open script file: '%s'\n", path);
		abort();
	}

	char* script_data = malloc(file->size + 1);
	memset(script_data, 0, file->size + 1);
	fread(script_data, file->size, 1, file);


	char current_line[256] = { 0 };
	int script_idx = 0;
	int idx = 0;

next:
	idx = 0;
	while (true) {
		if (script_idx >= file->size) {
			break;
		}

		if (script_data[script_idx] == '\n') {
			current_line[idx] = '\0';
			script_idx++;
			break;
		} else {
			current_line[idx] = script_data[script_idx];
		}

		script_idx++;
		idx++;
	}

	if (current_line[0] == '#') {
		goto next;
	}

	if (current_line[0] == '\0') {
		goto next;
	}

	// printf("Running line: '%s'\n", current_line);

	char command[256] = { 0 };
	memcpy(command, current_line, strlen(current_line) + 1);

	bool should_break = command_received(command); //This should block while command is running.
	memset(current_line, 0, 256);

	if (!should_break && script_idx < file->size) {
		goto next;
	}

	fclose(file); // we still use the file object in the loop so we can only close it here.
	free(script_data);
	return;
}