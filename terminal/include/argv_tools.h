#pragma once

#include <stdbool.h>

enum pipe_type_e {
	NORMAL,
	PIPE_PROC,
    PIPE_FILE,
    AND_RUN
};

int get_command_type(char* command, int* token_pos);

char* read_env(char* in);

char** argv_split(char* str);
char** argv_env_process(char** in);
void free_argv(char** argv);

bool is_quote_open(char* command);

char* process_line(char* command, bool check_hashtag);