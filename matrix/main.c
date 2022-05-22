#include <sys/time.h>

#include <stdio.h>

#include <foxos/term.h>
#include <foxos/fox_graphics.h>

#define GRAPHICS_RENDERING_MODE

#define RAND_MAX 32767
unsigned long next = 0;

int rand() {
	if (next == 0) {
		next = _time();
	}
    next = next * 1103515245 + 12345;
    return (uint32_t) (next / 65536) % RAND_MAX + 1; 
}

uint8_t buffer[1000][1000] = {0};
uint8_t resets[1000] = {0};
uint64_t num_frames_rendered = 0;

uint64_t num_frames_rendered_in_10_seconds = 0;
long long start_time = 0;
int fps = 0;

psf1_font_t font;
point_t screen_size;

graphics_buffer_info_t graphics_buffer_info;

void frame() {
	if (start_time == 0) {
		start_time = _time();
	}

	int rows = screen_size.y / 16 - 2;
	int cols = screen_size.x / 8 - 1;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (i == 0) {
				buffer[i][j] = (buffer[i][j] + 1) % (resets[j] + 2);
				
				if(buffer[i][j] == 0) {
					resets[j] = (rand() % rows);
				}

			} else {
				if(buffer[i - 1][j] != 0) {
					if(buffer[i - 1][j] - 1 > 0) {
						buffer[i][j] = buffer[i - 1][j] - 1;
					} else {
						buffer[i][j] = 0;
					}
				} else {
					buffer[i][j] = 0;
				}
			}
		}
	}

#ifdef GRAPHICS_RENDERING_MODE
	fox_start_frame(&graphics_buffer_info, true);
#endif

	for (int i = 0; i < rows; i++) {
	#ifndef GRAPHICS_RENDERING_MODE
		set_cursor((point_t) { 0, i * 16 });
		set_color(0);
		clear_line();
	#endif

		for (int j = 0; j < cols; j++) {
			
			if (buffer[i][j]) {
			#ifndef GRAPHICS_RENDERING_MODE
				set_cursor((point_t) { j * 8, i * 16 });
				set_color(0xff000000 | ((0xff - buffer[i][j]) << 12));
				putchar('X');
			#else
				fox_draw_char(&graphics_buffer_info, j * 8, i * 16, 'X', 0xff000000 | ((0xff - buffer[i][j]) << 12), &font);
			#endif
			}

		}
	}

#ifdef GRAPHICS_RENDERING_MODE
	char buffer[512] = { 0 };
	sprintf(buffer, "frames: %d (%d fps next update in %d s)", num_frames_rendered + 1, fps, start_time + 10 - _time());
	fox_draw_string(&graphics_buffer_info, 0, screen_size.y - 16, buffer, 0xffffffff, &font);

	fox_end_frame(&graphics_buffer_info);
#endif

	num_frames_rendered++;

#ifndef GRAPHICS_RENDERING_MODE
	set_color(0);
	set_cursor((point_t) { 0, screen_size.y - 16 });
	clear_line();
	set_color(0xffffffff);
	printf("frames: %d (%d fps next update in %d s)", num_frames_rendered, fps, start_time + 10 - _time());
#endif

	if (start_time + 10 - _time() < 0) {
		start_time = 0;
		fps = num_frames_rendered_in_10_seconds / 30;
		num_frames_rendered_in_10_seconds = 0;
	} else {
		num_frames_rendered_in_10_seconds++;
	}
}

int main() {
	graphics_buffer_info = create_screen_buffer();

	char font_load_path[512] = { 0 };
	sprintf(font_load_path, "%s/RES/zap-light16.psf", getenv("ROOT_FS"));
	font = fox_load_font(font_load_path);

	screen_size = get_screen_size();
	
	while (1) {
		frame();
	}

	fox_free_framebuffer(&graphics_buffer_info);
	fox_free_font(&font);

	return 0;
}