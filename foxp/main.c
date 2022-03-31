#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <fox_graphics.h>
#include <term.h>

typedef struct {
	uint64_t width;
	uint64_t height;
	uint32_t pixels[];
} fpic_image_t;

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

	void* buffer = malloc(f->size);
	fread(buffer, f->size, 1, f);
	fclose(f);

	fpic_image_t* image = (fpic_image_t*) buffer;

	// printf("Image: %d x %d\n", image->width, image->height);

	fox_start_frame(true);

	for (int y = 0; y < image->height; y++) {
		for (int x = 0; x < image->width; x++) {
			uint32_t pixel = image->pixels[y * image->width + x];
			fox_set_px(x, y, pixel);
		}
	}
	
	char font_load_path[512] = { 0 };
	sprintf(font_load_path, "%s/RES/zap-light16.psf", getenv("ROOT_FS"));
	psf1_font_t font = fox_load_font(font_load_path);
	fox_draw_string(0, image->height + 16, "Press any key to exit", 0xFFFFFFFF, &font);

	fox_end_frame();

	getchar();

	fox_start_frame(true);
	fox_draw_string(0, 0, "Goodbye!", 0xffffffff, &font);
	fox_end_frame();

	set_cursor((struct point_t) { 0, 16 });
}