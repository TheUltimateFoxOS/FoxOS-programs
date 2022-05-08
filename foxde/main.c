#include <renderer/mouse_renderer.h>
#include <renderer/task_bar_renderer.h>
#include <renderer/util_bar_renderer.h>

#include <utils/rainbow.h>

#include <config.h>

#include <fox_graphics.h>
#include <term.h>
#include <psf1_font.h>

//Variables from config.h
struct point_t screen_size;
psf1_font_t screen_font;

int main(int argc, char* argv[], char* envp[]) {
	disable_print_char();

	//Setup config variables to use in the program
	screen_size = get_screen_size();

	char font_load_path[512] = { 0 };
	sprintf(font_load_path, "%s/RES/zap-light16.psf", getenv("ROOT_FS"));
	screen_font = fox_load_font(font_load_path);

	//Main draw loop

	while (true) {
		fox_start_frame(true);

		fox_set_background(background_colour);
		draw_task_bar();
		draw_util_bar();

		draw_mouse_pointer();
		
		fox_end_frame();
	}

	enable_print_char();
	return 0;
}
