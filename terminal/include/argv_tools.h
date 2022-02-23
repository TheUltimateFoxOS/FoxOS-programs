#pragma once

char* read_env(char* in);
char** argv_split(char* str);
char** argv_env_process(char** in);
void free_argv(char** argv);