#include <g_syscalls.h>

#include <sys/get_syscall_id.h>
#include <string.h>

int sys_fb_info_id = -1;
int sys_copy_to_fb_id = -1;
int sys_copy_from_fb_id = -1;
int sys_mouse_get_buttons = -1;
int sys_mouse_get_pos = -1;
int sys_mouse_reset = -1;

struct framebuffer_t fb_info() {
	if (sys_fb_info_id == -1) {
		sys_fb_info_id = get_syscall_id("sys_fb_info");
	}

	struct framebuffer_t fb;
	memset(&fb, 0, sizeof(fb));

	__asm__ __volatile__ ("int $0x30" : : "a" (sys_fb_info_id), "b" (&fb));

	return fb;
}

void copy_to_fb(void* addr) {
	if (sys_copy_to_fb_id == -1) {
		sys_copy_to_fb_id = get_syscall_id("sys_copy_to_fb");
	}

	__asm__ __volatile__ ("int $0x30" : : "a" (sys_copy_to_fb_id), "b" (addr));
}

void copy_from_fb(void* addr) {
	if (sys_copy_from_fb_id == -1) {
		sys_copy_from_fb_id = get_syscall_id("sys_copy_from_fb");
	}

	__asm__ __volatile__ ("int $0x30" : : "a" (sys_copy_from_fb_id), "b" (addr));
}

void mouse_reset() {
	if (sys_mouse_reset == -1) {
		sys_mouse_reset = get_syscall_id("sys_mouse_reset");
	}

	__asm__ __volatile__ ("int $0x30" : : "a" (sys_mouse_reset));
}

struct mouse_position_t mouse_position() {
	if (sys_mouse_get_pos == -1) {
		sys_mouse_get_pos = get_syscall_id("sys_mouse_get_pos");
	}

	int x, y;

	__asm__ __volatile__ ("int $0x30" : "=b" (x), "=c" (y) : "a" (sys_mouse_get_pos));
	return (struct mouse_position_t) {x, y};
}

int mouse_button() {
	if (sys_mouse_get_buttons == -1) {
		sys_mouse_get_buttons = get_syscall_id("sys_mouse_get_buttons");
	}

	int button_left = 0;
	int button_right = 0;
	int button_middle = 0;

	__asm__ __volatile__ ("int $0x30" : "=b" (button_left), "=c" (button_right), "=d" (button_middle) : "a" (sys_mouse_get_buttons));

	if (button_left) {
		return MOUSE_BUTTON_LEFT;
	} else if (button_right) {
		return MOUSE_BUTTON_RIGHT;
	} else if (button_middle) {
		return MOUSE_BUTTON_MIDDLE;
	} else {
		return 0;
	}
}