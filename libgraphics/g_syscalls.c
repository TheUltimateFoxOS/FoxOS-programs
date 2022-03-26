#include <g_syscalls.h>

#include <sys/get_syscall_id.h>
#include <string.h>

int sys_fb_info_id = -1;
int sys_copy_to_fb_id = -1;
int sys_copy_from_fb_id = -1;

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