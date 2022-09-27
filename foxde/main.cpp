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
#include <launcher.h>

#include <sys/ipc.h>
#include <sys/env.h>
#include <sys/spawn.h>
#include <buildin/thread.h>

#include <foxos/fox_graphics.h>
#include <foxos/term.h>
#include <foxos/psf1_font.h>
#include <foxos/fox_graphics.h>
#include <cfg.h>
#include <foxdb.h>

//Variables from config.h
mouse_position_t mouse_pos;
int mouse_button_down;

psf1_font_t screen_font;
graphics_buffer_info_t graphics_buffer_info;
char* root_fs = nullptr;

//Other globals
special_keys_down_t* pressed_special_keys = 0;

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

#warning Very nasty way to do this, some keystrokes will be lost
uint8_t cache_signum = 0;
void special_key_sighandler(uint8_t signum) {
	cache_signum = signum;

	if (!pressed_special_keys) {
		pressed_special_keys = (special_keys_down_t*) env(ENV_GET_SPECIAL_KEYS);
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

	// char cfg_path[256];
	// sprintf(cfg_path, (char*) "%s/FOXCFG/foxde/config.cfg", root_fs);
	// printf("Loading config file: %s\n", cfg_path);
	// file_t* config_file = fopen(cfg_path, "r");
	SYSDB(sysdb, root_fs);
	if (sysdb == NULL) {
		printf("WARNING: Could not open sysdb (sys.fdb) file!\n");
	} else {
		if (foxdb_is_key(sysdb, "foxde_icons")) {
			foxdb_str_t* icons = foxdb_get_str(sysdb, "foxde_icons");

			// icons are , separated
			char* old_icons = icons->val;
			int icons_len = strlen(icons->val);
			for (int i = 0; i < icons_len; i++) {
				if (icons->val[i] == ',') {
					icons->val[i] = '\0';
					char* icon_cpy = (char*) malloc(strlen(old_icons) + 1);
					memset(icon_cpy, 0, strlen(old_icons) + 1);
					strcpy(icon_cpy, old_icons);
					add_icon(icon_cpy);
					old_icons = &icons->val[i + 1];
				}
			}

			char* icon_cpy = (char*) malloc(strlen(old_icons) + 1);
			memset(icon_cpy, 0, strlen(old_icons) + 1);
			strcpy(icon_cpy, old_icons);

			add_icon(icon_cpy);

			foxdb_del_entry((foxdb_entry_t*) icons);
		} else {
			printf("WARNING: key foxde_icons not found un sysdb (sys.fdb)!\n");
		}

		foxdb_del(sysdb);
	}

	task_t* self = (task_t*) env(ENV_GET_TASK);
	self->stdout_pipe = foxde_stdout;
	self->stderr_pipe = foxde_stderr;
	self->pipe_enabled = true;

	int ipc_id = ipc_register(standard_foxos_de_ipc_name, ipc_message);

#ifdef enable_terminal
	load_terminal();
#endif

	launcher_init();

	env_set2(ENV_SIGHANDLER, SIG_SPECIAL_KEY_DOWN, (void*) special_key_sighandler);
	env_set2(ENV_SIGHANDLER, SIG_SPECIAL_KEY_UP, (void*) special_key_sighandler);

	printf("Welcome to FoxDE!\n");
	printf("Copyright (c) 2022 TheUltimateFoxOS\n");

	bool on_terminal_task_exit = false;
#ifdef enable_terminal
	start_terminal(&on_terminal_task_exit, envp);
#endif

	init_window_renderer();

	//Main draw loop
	while (!on_terminal_task_exit) {
		launcher_tick();
		
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
			bool handled = mouse_handle_icons(mouse_pos.x, mouse_pos.y, (mouse_buttons_e) mouse_button_down);

			if (!handled) {
				mouse_handle_windows(mouse_pos.x, mouse_pos.y, (mouse_buttons_e) mouse_button_down);
			}
		}
		draw_mouse_pointer();

		if (cache_signum) {
			standard_foxos_window_t* window = get_front_window();
			if (window != nullptr) {
				if (cache_signum == SIG_SPECIAL_KEY_DOWN) {
					window->send_special_key_down(pressed_special_keys->triggered_by);
				} else if (cache_signum == SIG_SPECIAL_KEY_UP) {
					window->send_special_key_up(pressed_special_keys->triggered_by);
				}
			}

			cache_signum = 0;
		}
		
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