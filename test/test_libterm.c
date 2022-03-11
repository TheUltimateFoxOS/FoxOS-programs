#include <test_libterm.h>

#include <term.h>
#include <stdio.h>

int libterm(int argc, char* argv[], char* envp[]) {
	uint64_t old_color = get_color();
	set_color(0);
	
	clear_screen();

	set_color(0xffe36d2d);

	for (int i = 0; i < 10; i++) {
		printf("Hello, world!\n");
	}

	set_color(old_color);

	struct point_t old_cursor = get_cursor();
	set_cursor((struct point_t) { 100, 100 });

	set_color(0xffff0000);
	clear_line();
	set_color(old_color);
	printf("Hello, world (im made using set_cursor :o)!\n");

	set_cursor(old_cursor);

	struct point_t screen_size = get_screen_size();
	printf("Screen size: %d x %d\n", screen_size.x, screen_size.y);
}