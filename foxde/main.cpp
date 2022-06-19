#include <renderer/mouse_renderer.h>
#include <renderer/task_bar_renderer.h>
#include <renderer/util_bar_renderer.h>
#include <renderer/window_renderer.h>
#include <renderer/background_renderer.h>

#include <utils/rainbow.h>
#include <utils/bmp.h>

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

//Global variables
char* background_console = nullptr;
int background_console_colum = 0;
int background_console_row = 0;
int num_background_console_colums = 0;
int num_background_console_rows = 0;

void scroll_bg_console() {
	for (int i = 0; i < num_background_console_rows - 1; i++) {
		for (int j = 0; j < num_background_console_colums; j++) {
			background_console[i * num_background_console_colums + j] = background_console[(i + 1) * num_background_console_colums + j];
		}
	}

	for (int i = 0; i < num_background_console_colums; i++) {
		background_console[(num_background_console_rows - 1) * num_background_console_colums + i] = 0;
	}

	background_console_row--;
}

void print_bg_console(char* s, int l) {
	int i = 0;
	while (i < l) {
		if (s[i] == '\n') {
			background_console_colum = 0;
			background_console_row++;
			if (background_console_row >= num_background_console_rows) {
				scroll_bg_console();
			}

		} else if (s[i] == '\b') {
			if (background_console_colum > 0) {
				background_console_colum--;
			}

			background_console[(background_console_row * num_background_console_colums) + background_console_colum] = ' ';
		} else {
			background_console[background_console_colum + (background_console_row * num_background_console_colums)] = s[i];
			background_console_colum++;
			if (background_console_colum >= num_background_console_colums) {
				background_console_colum = 0;
				background_console_row++;
				if (background_console_row >= num_background_console_rows) {
					scroll_bg_console();
				}
			}
		}
		i++;
	}
}

void on_stdout(char* buffer, uint64_t size) {
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
	print_bg_console(buffer, size);
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1); // re-enable the pipe
}

void on_stderr(char* buffer, uint64_t size) {
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
	print_bg_console(buffer, size);
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1); // re-enable the pipe
}

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
				task->stdout_pipe = on_stdout;
				task->stderr_pipe = on_stderr;

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

	//Setup config variables to use in the program
	graphics_buffer_info = create_screen_buffer();
	if (graphics_buffer_info.buffer == NULL) {
		printf("Could not allocate memory for the graphics buffer\n");
		return 1;
	}

	char font_path[256];
	sprintf(font_path, (char*) "%s/RES/zap-light16.psf", root_fs);
	screen_font = fox_load_font(font_path);

	num_background_console_colums = graphics_buffer_info.width / font_width - bg_console_offset_width;
	num_background_console_rows = graphics_buffer_info.height / font_height - bg_console_offset_height;
	background_console = (char*) malloc(num_background_console_colums * num_background_console_rows);
	memset(background_console, 0, sizeof(char) * num_background_console_colums * num_background_console_rows);

	load_background_image();

	task_t* self = (task_t*) env(ENV_GET_TASK);
	self->stdout_pipe = on_stdout;
	self->stderr_pipe = on_stderr;
	self->pipe_enabled = true;

	printf("Welcome to FoxDE!\n");
	printf("Copyright (c) 2022 TheUltimateFoxOS\n");

	int ipc_id = ipc_register(standard_foxos_de_ipc_name, ipc_message);

	char* nargv[2];
	nargv[0] = (char*) startup_task;
	nargv[1] = nullptr;

	char startup_task_path[256];
	sprintf(startup_task_path, (char*) "%s/BIN/%s.elf", root_fs, startup_task);
	task_t* terminal_task = spawn(startup_task_path, (const char**) nargv, (const char**) envp, true);

	assert(terminal_task != NULL);
	terminal_task->stdout_pipe = on_stdout;
	terminal_task->stderr_pipe = on_stderr;
	terminal_task->pipe_enabled = true;

	bool on_terminal_task_exit = false;
	terminal_task->on_exit = &on_terminal_task_exit;

	//Main draw loop
	while (!on_terminal_task_exit) {
		fox_start_frame(&graphics_buffer_info, true);

		draw_background();

		for (int x = 0; x < num_background_console_colums; x++) {
			for (int y = 0; y < num_background_console_rows; y++) {
				if (background_console[(y * num_background_console_colums) + x] == 0) {
					continue;
				}

				fox_draw_char(&graphics_buffer_info, (x + bg_console_offset_width) * font_width, (y + bg_console_offset_height) * font_height, background_console[x + (y * num_background_console_colums)], 0x0, &screen_font);
			}
		}

		draw_windows();

		draw_task_bar();
		draw_util_bar();

		mouse_button_down = mouse_button();
		mouse_pos = mouse_position();
		if (mouse_button_down) {
			mouse_handle_windows(mouse_pos.x, mouse_pos.y, mouse_button_down);
		}
		draw_mouse_pointer();
		
		fox_end_frame(&graphics_buffer_info);
	}

	destroy_all_windows();
	enable_print_char();

	uint64_t old_color = get_color();
	set_color(0);
	clear_screen();
	set_color(old_color);

	ipc_unregister(ipc_id);

	free(graphics_buffer_info.buffer);

	return 0;
}