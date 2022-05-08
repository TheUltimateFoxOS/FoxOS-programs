#include <renderer/mouse_renderer.h>

#include <utils/rainbow.h>

#include <fox_graphics.h>
#include <g_syscalls.h>

#define mouse_pointer_width 10
#define mouse_pointer_height 16
#define mouse_pointer_len (mouse_pointer_width * mouse_pointer_height)

uint8_t mouse_pointer[] = {
	1,0,0,0,0,0,0,0,0,0,
	1,1,0,0,0,0,0,0,0,0,
	1,1,1,0,0,0,0,0,0,0,
	1,1,1,1,0,0,0,0,0,0,
	1,1,1,1,1,0,0,0,0,0,
	1,1,1,1,1,1,0,0,0,0,
	1,1,1,1,1,1,1,0,0,0,
	1,1,1,1,1,1,1,1,0,0,
	1,1,1,1,1,1,1,1,1,0,
	1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,0,0,0,0,
	1,1,1,0,1,1,0,0,0,0,
	1,1,0,0,0,1,1,0,0,0,
	1,0,0,0,0,1,1,0,0,0,
	0,0,0,0,0,0,1,1,0,0,
	0,0,0,0,0,0,1,1,0,0
};

void draw_mouse_pointer(uint32_t colour) {
    struct mouse_position_t pos = mouse_position();

	int current_x = 0;
	int current_y = 0;

	for (int i = 0; i < mouse_pointer_len; i++) {
		if (mouse_pointer[i] == 1) {
			fox_set_px(pos.x + current_x, pos.y + current_y, colour);
		}

		current_x++;
		if (current_x >= mouse_pointer_width) {
			current_x = 0;
			current_y++;
		}
	}
}