#include <renderer/mouse_renderer.h>
#include <renderer/task_bar_renderer.h>
#include <renderer/util_bar_renderer.h>
#include <renderer/window_renderer.h>

#include <utils/rainbow.h>

#include <config.h>

#include <foxos/fox_graphics.h>
#include <foxos/term.h>
#include <foxos/psf1_font.h>

//Variables from config.h
mouse_position_t mouse_pos;
int mouse_button_down;

screen_data_t screen_size;
psf1_font_t screen_font;

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

	window_t* window1 = new window_t(100, 100, 400, 200);
	window1->set_title((char*) "Window 1");
	register_window(window1);

	window_t* window2 = new window_t(200, 200, 400, 200);
	window2->set_title((char*) "Window 2");
	register_window(window2);

	window_t* window3 = new window_t(300, 300, 400, 200);
	window3->set_title((char*) "Window 3");
	register_window(window3);

	bring_window_to_front(window2);

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

		window2->move(window2->get_x() + 1, window2->get_y() + 1);
	}

	destroy_all_windows();
	enable_print_char();

	uint64_t old_color = get_color();
	set_color(0);
	clear_screen();
	set_color(old_color);

	return 0;
}
