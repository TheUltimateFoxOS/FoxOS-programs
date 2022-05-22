#include <test_graphics.h>

#include <foxos/term.h>
#include <foxos/fox_graphics.h>

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

	graphics_buffer_info_t graphics_buffer_info = create_screen_buffer();

	fox_start_frame(&graphics_buffer_info, true);

	fox_draw_rect(&graphics_buffer_info, 0, 0, 50, 50, 0xFF0000);
	fox_draw_string(&graphics_buffer_info, 55, 25, "This is a rectangle!", 0x0000FF, &font);

	fox_draw_circle(&graphics_buffer_info, 25, 60 + 25, 25, 0x0000FF);
	fox_draw_string(&graphics_buffer_info, 55, 60 + 25, "This is a circle!", 0x0000FF, &font);

	fox_draw_line(&graphics_buffer_info, 0, 120, 50, 120 + 50, 0x0000FF);
	fox_draw_string(&graphics_buffer_info, 55, 120 + 25, "This is a line!", 0x0000FF, &font);

	fox_draw_string(&graphics_buffer_info, 0, 190, "Press any key to exit.", 0x0000FF, &font);
	fox_end_frame(&graphics_buffer_info);

	getchar();

	fox_start_frame(&graphics_buffer_info, true);
	fox_draw_string(&graphics_buffer_info, 0, 0, "Goodbye!", 0x0000FF, &font);
	fox_end_frame(&graphics_buffer_info);

	set_cursor((point_t) { 0, 16 });

	fox_free_framebuffer(&graphics_buffer_info);
	fox_free_font(&font);

	// struct mouse_position_t last_pos = { 0, 0 };
	// while (true) {
	// 	struct mouse_position_t pos = mouse_position();
	// 	if (pos.x != last_pos.x || pos.y != last_pos.y) {
	// 		fox_start_frame(true);
	// 		fox_draw_rect(pos.x, pos.y, 10, 10, 0xFF0000);
	// 		fox_end_frame();
	// 		last_pos = pos;
	// 	}
	// }
}
