#pragma once

#include <stdint.h>

typedef struct {
	int x;
	int y;
	char* text;

	uint32_t* buffer;
	uint64_t buffer_size;
	void* window;

	bool* frame_ready;
} window_create_ipc_t;

enum ipc {
	IPC_CREATE_WINDOW = 1,
};