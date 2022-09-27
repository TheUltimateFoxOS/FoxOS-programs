#pragma once

typedef struct {
	char* name;
	char* help;
	void* (*exec)(int argc, char** argv, void* db);
} command_t;

#define no_args() if (argc != 1) { printf("Command takes no arguments!\n"); return false; }
#define x_args(x) if (argc != x + 1) { printf("Command takes %d arguments!\n", x); return false; }
#define x_or_more_args(x) if (argc < x + 1) { printf("Command takes %d+ arguments!\n", x); return false; }

void* keys(int argc, char** argv, void* db);
void* help(int argc, char** argv, void* db);
void* get(int argc, char** argv, void* db);
void* remove_(int argc, char** argv, void* db);
void* new_str(int argc, char** argv, void* db);
void* new_int(int argc, char** argv, void* db);
void* new_bool(int argc, char** argv, void* db);
void* append(int argc, char** argv, void* db);