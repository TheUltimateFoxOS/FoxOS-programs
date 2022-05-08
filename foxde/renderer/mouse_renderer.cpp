#include <renderer/mouse_renderer.h>

#include <utils/rainbow.h>

#include <foxos/fox_graphics.h>
#include <foxos/g_syscalls.h>

#define mouse_pointer_width 12
#define mouse_pointer_height 19
#define mouse_pointer_len (mouse_pointer_width * mouse_pointer_height)

uint8_t mouse_pointer[] = {
	1,0,0,0,0,0,0,0,0,0,0,0,
	1,1,0,0,0,0,0,0,0,0,0,0,
	1,2,1,0,0,0,0,0,0,0,0,0,
	1,2,2,1,0,0,0,0,0,0,0,0,
	1,2,2,2,1,0,0,0,0,0,0,0,
	1,2,2,2,2,1,0,0,0,0,0,0,
	1,2,2,2,2,2,1,0,0,0,0,0,
	1,2,2,2,2,2,2,1,0,0,0,0,
	1,2,2,2,2,2,2,2,1,0,0,0,
	1,2,2,2,2,2,2,2,2,1,0,0,
	1,2,2,2,2,2,2,2,2,2,1,0,
	1,2,2,2,2,2,2,2,2,2,2,1,
	1,2,2,2,2,2,2,1,1,1,1,1,
	1,2,2,2,1,2,2,1,0,0,0,0,
	1,2,2,1,0,1,2,2,1,0,0,0,
	1,2,1,0,0,1,2,2,1,0,0,0,
	1,1,0,0,0,0,1,2,2,1,0,0,
	0,0,0,0,0,0,1,2,2,1,0,0,
	0,0,0,0,0,0,0,1,1,0,0,0
};

void draw_mouse_pointer() {
    mouse_position_t pos = mouse_position();

	int current_x = 0;
	int current_y = 0;

	for (int i = 0; i < mouse_pointer_len; i++) {
		if (mouse_pointer[i] == 1) {
			fox_set_px(pos.x + current_x, pos.y + current_y, 0xFFFFFF);
		} else if (mouse_pointer[i] == 2) {
			fox_set_px(pos.x + current_x, pos.y + current_y, 0x000000);
		}

		current_x++;
		if (current_x >= mouse_pointer_width) {
			current_x = 0;
			current_y++;
		}
	}
}