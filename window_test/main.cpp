#include <stdio.h>
#include <stdint.h>

#include <foxos/fox_graphics.h>
#include <foxos/g_syscalls.h>
#include <foxos/window.h>

int main(int argc, char* argv[], char* envp[]) {
	standard_foxos_window_t* window = new standard_foxos_window_t(100, 100, 200, 200, (char*) "My very long window name to try and reach the maximum limit allowed to render");
	window->add_click_listner([](int64_t mouse_x, int64_t mouse_y, mouse_buttons_e mouse_button) {
		printf("Button 1 pressed %d\n", mouse_button);
	}, 20, 20, 10, 10);

	window->add_click_listner([](int64_t mouse_x, int64_t mouse_y, mouse_buttons_e mouse_button) {
		printf("Button 2 pressed %d\n", mouse_button);
	}, 100, 100, 30, 30);

	window->add_click_listner([](int64_t mouse_x, int64_t mouse_y, mouse_buttons_e mouse_button) {
		printf("Window clicked %d\n", mouse_button);
	});

	bool did_register = fox_register_window(window);
	if (!did_register) {
		printf("Failed to register window, there is no desktop environment running!\n");
		return 1;
	}

	window->exit_button = true;

	graphics_buffer_info_t buffer_info = window->get_buffer_info();

	char font_load_path[512] = { 0 };
	sprintf(font_load_path, "%s/RES/zap-light16.psf", getenv("ROOT_FS"));
	psf1_font_t font = fox_load_font(font_load_path);

	uint32_t color = 0;

	printf("Going into loop\n");

	while (!window->should_exit) {
		fox_start_frame(&buffer_info, true);

		fox_set_background(&buffer_info, color);
		color += 0xff;

		fox_draw_string(&buffer_info, 0, 0, (char*) "Hello!", 0xffffffff, &font);
		
		fox_draw_rect(&buffer_info, 20, 20, 10, 10, 0xffffffff); // visualize the button
		fox_draw_rect(&buffer_info, 100, 100, 30, 30, 0xffffffff); // visualize the button

		fox_draw_circle(&buffer_info, 50, 50, 10, 0xffffffff);

		fox_end_frame(&buffer_info);
	}

	printf("Exiting loop\n");

	fox_unregister_window(window);
	delete window;
	fox_free_font(&font);

	return 0;
}