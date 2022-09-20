#include <renderer/task_bar_renderer.h>

#include <foxos/fox_graphics.h>
#include <foxos/list.h>

#include <utils/bmp.h>
#include <config.h>
#include <launcher.h>

#include <assert.h>

int max_displayable_icons = -1;
list_t<task_bar_icon_t>* task_bar_icons = 0;

bool add_icon(char* name) {
	if (max_displayable_icons == -1) {
		max_displayable_icons = (graphics_buffer_info.height - task_bar_offset_y) / (task_bar_icon_size + task_bar_icon_padding);
	}
	if (!task_bar_icons) {
		task_bar_icons = new list_t<task_bar_icon_t>(max_displayable_icons);
	}

	task_bar_icon_t icon;
	icon.name = name;

	icon.x = task_bar_icon_padding;
	icon.y = task_bar_icons->get_length() * (task_bar_icon_size + task_bar_icon_padding) + task_bar_offset_y + task_bar_icon_padding;
	icon.width = task_bar_icon_size;
	icon.height = task_bar_icon_size;

	char image_path[256];
	sprintf(image_path, "%s/FOXCFG/foxde/%s.bmp", root_fs, name);
	printf("Loading icon: %s\n", image_path);

	file_t* image = fopen(image_path, "rb");
	if (image == NULL) {
		return false;
	}

	icon.img_buffer = (uint32_t*) malloc(icon.width * icon.height * sizeof(uint32_t));

	uint8_t* buffer = (uint8_t*) malloc(image->size);
	fread(buffer, image->size, 1, image);
	fclose(image);

	if (!is_bmp(buffer)) {
		return false;
	}

	int width = get_bmp_width(buffer);
	int height = get_bmp_height(buffer);

	for (int i = 0; i < icon.height; i++) {
		for (int j = 0; j < icon.width; j++) {
			icon.img_buffer[(i * icon.width) + j] = get_bmp_pixel(buffer, j, i);
		}
	}

	free(buffer);

	task_bar_icons->add(icon);

	return true;
}

void draw_icon(task_bar_icon_t* icon) {
	for (int i = 0; i < icon->height; i++) {
		for (int j = 0; j < icon->width; j++) {
			fox_set_px_unsafe(&graphics_buffer_info, icon->x + j, icon->y + i, icon->img_buffer[(i * icon->width) + j]);
		}
	}
	
	fox_draw_rect_outline(&graphics_buffer_info, icon->x, icon->y, icon->width, icon->height, 0xFFFFFF);
}

void draw_task_bar() {
    fox_draw_rect(&graphics_buffer_info, 0, 0 + task_bar_offset_y, task_bar_width, graphics_buffer_info.width - task_bar_offset_y, task_bar_colour);

	task_bar_icons->foreach([](list_t<task_bar_icon_t>::node* node) {
		draw_icon(&node->data);
	});
}

bool mouse_handle_icons(int64_t mouse_x, int64_t mouse_y, mouse_buttons_e mouse_button) {
	struct lambda_callback_data_t {
        int64_t mouse_x;
        int64_t mouse_y;
        mouse_buttons_e mouse_button;

        bool handled;
    };

    lambda_callback_data_t data;
    data.mouse_x = mouse_x;
    data.mouse_y = mouse_y;
    data.mouse_button = mouse_button;
    data.handled = false;

	task_bar_icons->foreach([](list_t<task_bar_icon_t>::node* node, void* lambda_data) {
        lambda_callback_data_t* data = (lambda_callback_data_t*) lambda_data;
		task_bar_icon_t* icon = &node->data;

        if (data->handled) {
            return;
        }

		if (data->mouse_x >= icon->x && data->mouse_x <= icon->x + icon->width && data->mouse_y >= icon->y && data->mouse_y <= icon->y + icon->height) {
			if (data->mouse_button == MOUSE_BUTTON_LEFT) {
				launcher_run(icon->name);
			}

			data->handled = true;
		}
	}, &data);

	return data.handled;
}