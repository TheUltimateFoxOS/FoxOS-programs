#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <foxos/fox_graphics.h>
#include <foxos/term.h>

#include <image_reader.h>

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s <file>\n", argv[0]);
		return 1;
	}

	FILE *f = fopen(argv[1], "r");
	if (!f) {
		printf("Could not open file %s\n", argv[1]);
		return 1;
	}

	graphics_buffer_info_t graphics_buffer_info = create_screen_buffer();

	void* buffer = malloc(f->size);
	fread(buffer, f->size, 1, f);
	fclose(f);

	image_reader_t* reader = image_reader_detect(buffer);
	assert(reader != NULL);

	fox_start_frame(&graphics_buffer_info, true);

	int width = reader->get_width(buffer);
	int height = reader->get_height(buffer);

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			uint32_t pixel = reader->get_pix(buffer, x, y);
			if (graphics_buffer_info.width < x || graphics_buffer_info.height - 16 < y) {
				continue;
			}

			fox_set_px(&graphics_buffer_info, x, y, pixel);
		}
	}

	char font_load_path[512] = { 0 };
	sprintf(font_load_path, "%s/RES/zap-light16.psf", getenv("ROOT_FS"));
	psf1_font_t font = fox_load_font(font_load_path);
	if (graphics_buffer_info.width < width || graphics_buffer_info.height < height) {
		fox_draw_string(&graphics_buffer_info, 0, graphics_buffer_info.height - 16, "Press any key to exit (Image was too big, rendered as much as possible)", 0xffffffff, &font);
	} else {
		fox_draw_string(&graphics_buffer_info, 0, height + 16, "Press any key to exit", 0xFFFFFFFF, &font);
	}

	fox_end_frame(&graphics_buffer_info);

	getchar();

	fox_start_frame(&graphics_buffer_info, true);
	fox_draw_string(&graphics_buffer_info, 0, 0, "Goodbye!", 0xffffffff, &font);
	fox_end_frame(&graphics_buffer_info);

	set_cursor((point_t) { 0, 16 });

	fox_free_framebuffer(&graphics_buffer_info);
	fox_free_font(&font);
	free(buffer);
}