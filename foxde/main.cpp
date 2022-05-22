#include <renderer/mouse_renderer.h>
#include <renderer/task_bar_renderer.h>
#include <renderer/util_bar_renderer.h>
#include <renderer/window_renderer.h>

#include <utils/rainbow.h>

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

void on_stdout(char* buffer, uint64_t size) {
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1); // re-enable the pipe
}

void on_stderr(char* buffer, uint64_t size) {
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1); // re-enable the pipe
}

void ipc_message(int func, void* data) {
	switch (func) {
		case IPC_GET_WINDOW_INFO:
			{
				standard_foxos_window_info_t* window_info = (standard_foxos_window_info_t*) data;
				window_info->buffer_width_diff = window_buffer_width_diff;
				window_info->buffer_height_diff = window_buffer_height_diff;
				window_info->buffer_offset_x = window_buffer_offset_x;
				window_info->buffer_offset_y = window_buffer_offset_y;

				window_info->min_width = window_min_width;
				window_info->min_height = window_min_height;

				window_info->background_colour = window_background_colour;
			}
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
	if (de_running()) {
		printf("Error: a desktop environment is already running!\n");
		return 1;
	}

	disable_print_char();

	//Setup config variables to use in the program
	graphics_buffer_info = create_screen_buffer();
	if (graphics_buffer_info.buffer == NULL) {
		printf("Could not allocate memory for the graphics buffer\n");
		return 1;
	}

	char font_load_path[512] = { 0 };
	sprintf(font_load_path, "%s/RES/zap-light16.psf", getenv("ROOT_FS"));
	screen_font = fox_load_font(font_load_path);

	int ipc_id = ipc_register(standard_foxos_de_ipc_name, ipc_message);

	char* nargv[] = {
		"window_test",
		nullptr
	};

	spawn("root:/BIN/window_test.elf", (const char**) nargv, (const char**) envp, true);

	//Main draw loop
	while (true) {
		fox_start_frame(&graphics_buffer_info, true);

		fox_set_background(&graphics_buffer_info, main_background_colour);

		draw_windows();

		draw_task_bar();
		draw_util_bar();

		mouse_button_down = mouse_button();
		mouse_pos = mouse_position();
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
