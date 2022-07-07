#include <renderer/background_renderer.h>

#include <assert.h>
#include <stdint.h>

#include <utils/bmp.h>
#include <config.h>

#ifdef enable_background
uint32_t* background_image = nullptr;

void load_background_image() {
    background_image = (uint32_t*) malloc(graphics_buffer_info.width * graphics_buffer_info.height * sizeof(uint32_t));
	memset(background_image, 0xff, graphics_buffer_info.width * graphics_buffer_info.height * sizeof(uint32_t));

	char background_image_path[256];
	sprintf(background_image_path, "%s/FOXCFG/foxde-bg.bmp", root_fs);
	file_t* background_image_file = fopen(background_image_path, "rb");

	if (background_image_file != NULL) {
		uint8_t* buffer = (uint8_t*) malloc(background_image_file->size);
		fread(buffer, 1, background_image_file->size, background_image_file);
		fclose(background_image_file);

		assert(is_bmp(buffer));

		int width = get_bmp_width(buffer);
		int height = get_bmp_height(buffer);

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (i >= graphics_buffer_info.height || j >= graphics_buffer_info.width) {
					buffer[(i * width) + j] = 0;
				} else {
					background_image[(i * graphics_buffer_info.width) + j] = get_bmp_pixel(buffer, j, i);
				}
			}
		}

		free(buffer);
	}
}

void draw_background() {
    for (int i = 0; i < graphics_buffer_info.height; i++) {
		for (int j = 0; j < graphics_buffer_info.width; j++) {
			fox_set_px_unsafe(&graphics_buffer_info, j, i, background_image[(i * graphics_buffer_info.width) + j]);
		}
	}
}

#else
void draw_background() {
    for (int i = 0; i < graphics_buffer_info.height; i++) {
		for (int j = 0; j < graphics_buffer_info.width; j++) {
			fox_set_px_unsafe(&graphics_buffer_info, j, i, 0xffffff);
		}
	}
}
#endif