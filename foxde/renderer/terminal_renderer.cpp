#include <renderer/terminal_renderer.h>

#include <foxde_std.h>
#include <config.h>

#include <stdint.h>

#include <sys/env.h>
#include <sys/spawn.h>

#ifdef enable_terminal
char* terminal_argv[2];

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

void load_terminal() {
    num_background_console_colums = graphics_buffer_info.width / font_width - bg_console_offset_width;
	num_background_console_rows = graphics_buffer_info.height / font_height - bg_console_offset_height;
	background_console = (char*) malloc(num_background_console_colums * num_background_console_rows);
	memset(background_console, 0, sizeof(char) * num_background_console_colums * num_background_console_rows);
}

void start_terminal(bool* on_exit, char* envp[]) {
	terminal_argv[0] = (char*) startup_task;
	terminal_argv[1] = nullptr;

	char startup_task_path[256];
	sprintf(startup_task_path, (char*) "%s/BIN/%s.elf", root_fs, startup_task);
	task_t* terminal_task = spawn(startup_task_path, (const char**) terminal_argv, (const char**) envp, true);

	assert(terminal_task != NULL);
	terminal_task->stdout_pipe = foxde_stdout;
	terminal_task->stderr_pipe = foxde_stderr;
	terminal_task->pipe_enabled = true;

	terminal_task->on_exit = on_exit;
}

void draw_terminal() {
    for (int x = 0; x < num_background_console_colums; x++) {
		for (int y = 0; y < num_background_console_rows; y++) {
			if (background_console[(y * num_background_console_colums) + x] == 0) {
				continue;
			}

			fox_draw_char(&graphics_buffer_info, (x + bg_console_offset_width) * font_width, (y + bg_console_offset_height) * font_height, background_console[x + (y * num_background_console_colums)], 0x0, &screen_font);
		}
	}
}
#endif