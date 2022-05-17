#include <renderer/mouse_renderer.h>
#include <renderer/task_bar_renderer.h>
#include <renderer/util_bar_renderer.h>
#include <renderer/window_renderer.h>

#include <utils/rainbow.h>

#include <config.h>
#include <ipc.h>

#include <sys/ipc.h>
#include <sys/env.h>
#include <sys/spawn.h>
#include <buildin/thread.h>

#include <foxos/fox_graphics.h>
#include <foxos/term.h>
#include <foxos/psf1_font.h>

//Variables from config.h
mouse_position_t mouse_pos;
int mouse_button_down;

screen_data_t screen_size;
psf1_font_t screen_font;

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
		case IPC_CREATE_WINDOW:
			{
				window_create_ipc_t* ipc_data = (window_create_ipc_t*) data;
				window_t* window = new window_t(100, 100, ipc_data->x, ipc_data->y);
				window->set_title((char*) ipc_data->text);
				ipc_data->buffer = window->buffer;
				ipc_data->buffer_size = window->buffer_size;
				ipc_data->window = window;
				ipc_data->x = window->get_width() - 2;
				ipc_data->y = window->get_height() - window_bar_height;

				ipc_data->frame_ready = &window->frame_ready;

				task_t* task = (task_t*) env(ENV_GET_TASK);
				task->stdout_pipe = on_stdout;
				task->stderr_pipe = on_stderr;

				register_window(window);
			}
			break;
	}
}

int main(int argc, char* argv[], char* envp[]) {
	disable_print_char();

	//Setup config variables to use in the program
	point_t siz = get_screen_size();
	screen_size = {
		.width = (int64_t) siz.x,
		.height = (int64_t) siz.y
	};

	char font_load_path[512] = { 0 };
	sprintf(font_load_path, "%s/RES/zap-light16.psf", getenv("ROOT_FS"));
	screen_font = fox_load_font(font_load_path);

	// window_t* window1 = new window_t(100, 100, 400, 200);
	// window1->set_title((char*) "Window 1");
	// register_window(window1);

	// window_t* window2 = new window_t(200, 200, 400, 200);
	// window2->set_title((char*) "Window 2");
	// register_window(window2);

	// window_t* window3 = new window_t(300, 300, 400, 200);
	// for (int i = 0; i < window3->get_height(); i++) {
	// 	for (int j = 0; j < window3->get_width(); j++) {
	// 		window3->buffer[i * window3->get_width() + j] = rainbow(i * window3->get_width() + j);
	// 	}
	// }
	// window3->set_title((char*) "Window 3");
	// register_window(window3);

	// bring_window_to_front(window2);

	int ipc_id = ipc_register("foxde", ipc_message);

	char* nargv[] = {
		"ipc_test",
		nullptr
	};

	spawn("root:/BIN/ipc_test.elf", (const char**) nargv, (const char**) envp, true);

	//Main draw loop
	while (true) {
		fox_start_frame(true);

		fox_set_background(background_colour);

		draw_windows();

		draw_task_bar();
		draw_util_bar();

		mouse_button_down = mouse_button();
		mouse_pos = mouse_position();
		draw_mouse_pointer();
		
		fox_end_frame();

		// window2->move(window2->get_x() + 1, window2->get_y() + 1);
	}

	destroy_all_windows();
	enable_print_char();

	uint64_t old_color = get_color();
	set_color(0);
	clear_screen();
	set_color(old_color);

	ipc_unregister(ipc_id);

	return 0;
}
