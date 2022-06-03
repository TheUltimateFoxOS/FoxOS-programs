#include <stdio.h>
#include <stdint.h>

#include <foxos/fox_graphics.h>
#include <foxos/window.h>

int main(int argc, char* argv[], char* envp[]) {
	standard_foxos_window_t* window = new standard_foxos_window_t(100, 100, 200, 200, (char*) "IPC Test");

	bool did_register = fox_register_window(window);
	if (!did_register) {
		printf("Failed to register window, there is no desktop environment running!\n");
		return 1;
	}

	graphics_buffer_info_t buffer_info = window->get_buffer_info();

	char font_load_path[512] = { 0 };
	sprintf(font_load_path, "%s/RES/zap-light16.psf", getenv("ROOT_FS"));
	psf1_font_t font = fox_load_font(font_load_path);

	uint32_t color = 0;

	while (1) {
		while (window->frame_ready) {
			__asm__ __volatile__("pause" ::: "memory");
		}

		fox_start_frame(&buffer_info, true);

		fox_set_background(&buffer_info, color);
		color += 0xff;

		fox_draw_string(&buffer_info, 0, 0, (char*) "Hello!", 0xffffffff, &font);
		fox_draw_rect(&buffer_info, 20, 20, 10, 10, 0xffffffff);
		fox_draw_circle(&buffer_info, 50, 50, 10, 0xffffffff);

		window->frame_ready = true;
	}

	fox_unregister_window(window);
	delete window;
	fox_free_font(&font);

	return 0;
}