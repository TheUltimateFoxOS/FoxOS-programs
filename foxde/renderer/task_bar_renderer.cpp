#include <renderer/task_bar_renderer.h>

#include <foxos/fox_graphics.h>
#include <config.h>
#include <utils/bmp.h>
#include <assert.h>

icon_t icons[MAX_ICONS] = { 0 };
int num_icons = 0;

void add_icon(char* name) {
	if (num_icons >= MAX_ICONS) {
		return;
	}

	icon_t* icon = &icons[num_icons];
	icon->name = name;

	icon->x = task_bar_icon_padding;
	icon->y = num_icons * (task_bar_icon_size + task_bar_icon_padding) + task_bar_offset_y + task_bar_icon_padding;
	icon->width = task_bar_icon_size;
	icon->height = task_bar_icon_size;

	char image_path[256];
	sprintf(image_path, "%s/FOXCFG/%s.bmp", root_fs, name);
	printf("Loading icon: %s\n", image_path);
	file_t* image = fopen(image_path, "rb");
	assert(image != NULL);

	icon->img_buffer = (uint32_t*) malloc(icon->width * icon->height * sizeof(uint32_t));

	uint8_t* buffer = (uint8_t*) malloc(image->size);
	fread(buffer, 1, image->size, image);
	fclose(image);

	assert(is_bmp(buffer));

	int width = get_bmp_width(buffer);
	int height = get_bmp_height(buffer);

	for (int i = 0; i < icon->height; i++) {
		for (int j = 0; j < icon->width; j++) {
			icon->img_buffer[(i * icon->width) + j] = get_bmp_pixel(buffer, j, i);
		}
	}

	free(buffer);

	num_icons++;
}

void draw_icon(icon_t* icon) {
	for (int i = 0; i < icon->height; i++) {
		for (int j = 0; j < icon->width; j++) {
			fox_set_px_unsafe(&graphics_buffer_info, icon->x + j, icon->y + i, icon->img_buffer[(i * icon->width) + j]);
		}
	}
	
	fox_draw_rect_outline(&graphics_buffer_info, icon->x, icon->y, icon->width, icon->height, 0xFFFFFF);
}

void draw_task_bar() {
    fox_draw_rect(&graphics_buffer_info, 0, 0 + task_bar_offset_y, task_bar_width, graphics_buffer_info.width - task_bar_offset_y, task_bar_colour);

	for (int i = 0; i < num_icons; i++) {
		draw_icon(&icons[i]);
	}
}