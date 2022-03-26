#include <test_graphics.h>
#include <term.h>
#include <fox_graphics.h>

int graphics(int argc, char* argv[], char* envp[]) {
	psf1_font_t font = fox_load_font("RES/zap-light16.psf");
	// fox_start_frame(true);

	// // fox_draw_rect(0, 0, 320, 240, 0xFF0000);
	// fox_draw_circle(160, 120, 100, 0x0000FF);
	// // fox_draw_circle_filled(160, 120, 50, 0xFFFF00);

	// fox_draw_char(0, 0, 'A', 0x0000FF, &font);
	// fox_draw_string(0, 400, "Hello, world!", 0x0000FF, &font);

	// for (int i = 0; i < 100; i += 10) {
	// 	fox_draw_line(0, 0, i, 300, 0xFF0000);
	// }

	// fox_draw_line(0, 300, 0, 400, 0xFF0000);
	// fox_draw_line(0, 300, 40, 400, 0xFF0000);
	// fox_draw_line(0, 400, 40, 400, 0xFF0000);


	// fox_end_frame();

	fox_start_frame(true);

	fox_draw_rect(0, 0, 50, 50, 0xFF0000);
	fox_draw_string(55, 25, "This is a rectangle!", 0x0000FF, &font);

	fox_draw_circle(25, 60 + 25, 25, 0x0000FF);
	fox_draw_string(55, 60 + 25, "This is a circle!", 0x0000FF, &font);

	fox_draw_line(0, 120, 50, 120 + 50, 0x0000FF);
	fox_draw_string(55, 120 + 25, "This is a line!", 0x0000FF, &font);

	fox_draw_string(0, 190, "Press any key to exit.", 0x0000FF, &font);
	fox_end_frame();

	getchar();

	fox_start_frame(true);
	fox_draw_string(0, 0, "Goodbye!", 0x0000FF, &font);
	fox_end_frame();

	set_cursor((struct point_t) { 0, 16 });
}
