#include <renderer/mouse_renderer.h>
#include <renderer/task_bar_renderer.h>
#include <renderer/util_bar_renderer.h>
#include <renderer/window_renderer.h>
#include <renderer/background_renderer.h>
#include <renderer/terminal_renderer.h>

#include <utils/rainbow.h>
#include <utils/bmp.h>

#include <foxde_std.h>
#include <config.h>

#include <sys/ipc.h>
#include <sys/env.h>
#include <sys/spawn.h>
#include <buildin/thread.h>

#include <foxos/fox_graphics.h>
#include <foxos/term.h>
#include <foxos/psf1_font.h>
#include <foxos/fox_graphics.h>

//Variables from config.h
mouse_position_t mouse_pos;
int mouse_button_down;

psf1_font_t screen_font;
graphics_buffer_info_t graphics_buffer_info;
char* root_fs = nullptr;

void ipc_message(int func, void* data) {
	switch (func) {
		case IPC_GET_WINDOW_INFO:
			{
				standard_foxos_window_info_t* window_info = (standard_foxos_window_info_t*) data;
				window_info->buffer_width_diff = window_buffer_width_diff;
				window_info->buffer_height_diff = window_buffer_height_diff;

				window_info->min_width = window_min_width;
				window_info->min_height = window_min_height;

				window_info->background_colour = window_background_colour;
			}
			break;
		case IPC_CREATE_WINDOW:
			{
				standard_foxos_window_t* new_window = (standard_foxos_window_t*) data;

				task_t* task = (task_t*) env(ENV_GET_TASK);
#ifdef enable_terminal
				task->stdout_pipe = foxde_stdout;
				task->stderr_pipe = foxde_stderr;
#endif

				register_window(new_window);
			}
			break;
		case IPC_DESTORY_WINDOW:
			{
				standard_foxos_window_t* window = (standard_foxos_window_t*) data;
				unregister_window(window);
			}
			break;
	}
}

int main(int argc, char* argv[], char* envp[]) {
	root_fs = getenv("ROOT_FS");
	if (de_running()) {
		printf("Error: a desktop environment is already running!\n");
		return 1;
	}

	printf("Starting desktop environment...\n");

	disable_print_char();
	lock_print_char();

	//Setup config variables to use in the program
	graphics_buffer_info = create_screen_buffer();
	if (graphics_buffer_info.buffer == NULL) {
		printf("Could not allocate memory for the graphics buffer\n");
		return 1;
	}

	char font_path[256];
	sprintf(font_path, (char*) "%s/RES/zap-light16.psf", root_fs);
	screen_font = fox_load_font(font_path);

#ifdef enable_background
	load_background_image();
#endif

	task_t* self = (task_t*) env(ENV_GET_TASK);
	self->stdout_pipe = foxde_stdout;
	self->stderr_pipe = foxde_stderr;
	self->pipe_enabled = true;

	printf("Welcome to FoxDE!\n");
	printf("Copyright (c) 2022 TheUltimateFoxOS\n");

	int ipc_id = ipc_register(standard_foxos_de_ipc_name, ipc_message);

	bool on_terminal_task_exit = false;
#ifdef enable_terminal
	load_terminal(&on_terminal_task_exit, envp);
#endif

	//Main draw loop
	while (!on_terminal_task_exit) {
		fox_start_frame(&graphics_buffer_info, true);

		draw_background();

#ifdef enable_terminal
		draw_terminal();
#endif

		draw_windows();

		draw_task_bar();
		draw_util_bar();

		mouse_button_down = mouse_button();
		mouse_pos = mouse_position();
		if (mouse_button_down) {
			mouse_handle_windows(mouse_pos.x, mouse_pos.y, (mouse_buttons_e) mouse_button_down);
		}
		draw_mouse_pointer();
		
		fox_end_frame(&graphics_buffer_info);
	}

	destroy_all_windows();

	unlock_print_char();
	enable_print_char();

	uint64_t old_color = get_color();
	set_color(0);
	clear_screen();
	set_color(old_color);

	ipc_unregister(ipc_id);

	free(graphics_buffer_info.buffer);

	return 0;
}