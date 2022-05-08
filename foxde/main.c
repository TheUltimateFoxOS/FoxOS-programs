#include <renderer/mouse_renderer.h>

#include <utils/rainbow.h>

#include <fox_graphics.h>
#include <term.h>

int main(int argc, char* argv[], char* envp[]) {
	disable_print_char();

	struct point_t screen_size = get_screen_size();

	char font_load_path[512] = { 0 };
	sprintf(font_load_path, "%s/RES/zap-light16.psf", getenv("ROOT_FS"));
	psf1_font_t font = fox_load_font(font_load_path);

	int mouse_cidx = 0;
	int bg_cidx = 180;

	while (true) {
		fox_start_frame(true);
		
		fox_set_background(rainbow(++bg_cidx));
		draw_mouse_pointer(rainbow(++mouse_cidx));

		for (int i = -180; i <= 360; i++) {
			fox_draw_rect(i, i, 10, 10, rainbow(i));
		}

		bg_cidx += 2;
		mouse_cidx += 2;
		
		fox_end_frame();
	}
	return 0;
}
