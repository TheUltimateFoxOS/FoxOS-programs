#include <stdio.h>
#include <sys/ipc.h>
#include <sys/delay.h>
#include <assert.h>

#include <stdint.h>

#include <foxos/fox_graphics.h>

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

int main(int argc, char* argv[], char* envp[]) {
	int hid = ipc_get_hid("foxde");
	assert(hid != -1);

	window_create_ipc_t data = {
		.x = 200,
		.y = 200,
		.text = "ipc_test",
	};

	ipc_send_message(hid, IPC_CREATE_WINDOW, &data);


	global_fb.base_address = data.buffer;
	global_fb.width = data.x;
	global_fb.height = data.y;
	global_fb.buffer_size = data.buffer_size;

	global_fb_ptr = data.buffer;


	char font_load_path[512] = { 0 };
	sprintf(font_load_path, "%s/RES/zap-light16.psf", getenv("ROOT_FS"));
	psf1_font_t font = fox_load_font(font_load_path);


	uint32_t color = 0;

	while (1) {
		while (*data.frame_ready) { __asm__ __volatile__("pause" ::: "memory"); }
		fox_start_frame(true);

		fox_set_background(color);
		color += 0xff;

		fox_draw_string(0, 0, "Hello!", 0xffffffff, &font);
		fox_draw_rect(20, 20, 10, 10, 0xffffffff);
		fox_draw_circle(50, 50, 10, 0xffffffff);

		*data.frame_ready = true;
	}

	return 0;
}