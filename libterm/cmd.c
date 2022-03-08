#include <cmd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/write.h>
#include <sys/open.h>
#include <sys/close.h>

int font_renderer_device = 0;

void __libterm_send_command(char* command, int size) {
	if (font_renderer_device == 0) {
		font_renderer_device = open("dev:font_renderer");
		if (font_renderer_device == 0) {
			printf("Error: Could not open font renderer device.\n");
			abort();
		}

		__libc_set_shutdown_hook(__libterm_shutdown_hook);
	}

	write(font_renderer_device, command, size, 0);
}

void __libterm_shutdown_hook() {
	if (font_renderer_device != 0) {
		close(font_renderer_device);
	}
}