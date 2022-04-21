#include <render.h>
#include <term.h>

#include <stdarg.h>
#include <rainbow.h>
#include <string.h>
#include <stdio.h>

int cidx = 0;

int num_length(int num) {
	if (num == 0) {
		return 1;
	}

	int len = 0;
	while (num > 0) {
		num /= 10;
		len++;
	}
	return len;
}

uint32_t rainbow(int i) {
	return __builtin_bswap32(true_hsv(i % 360));
}

void write_rainbow(char* what) {
	for (int i = 0; i < strlen(what); i++) {
		uint32_t color = rainbow(++cidx * 4);
		set_color(color);
		putchar(what[i]);
	}
}

void printf_rainbow(char* format, ...) {
	cidx = 0;
	va_list args;
	va_start(args, format);

	char buffer[1024] = { 0 };
	vsprintf(buffer, format, args);

	write_rainbow(buffer);

	va_end(args);

	set_color(0xFFFFFF);
}

uint32_t old_color;

void setup_terminal() {
	old_color = get_color();
	set_color(0);
	clear_screen();
}

void render_list(task_t** tasks, int count, uint64_t total_memory_mb, uint64_t free_memory_mb, uint64_t used_memory_mb, uint64_t reserved_memory_mb) {
	struct point_t screen_size = get_screen_size();

	set_color(0);
	clear_screen();

	// render header (in the top middle of the screen)
	char* header_text = "FoxT The Task Manager";
	set_color(0xFFFFFF);
	set_cursor((struct point_t) { .x = (screen_size.x - strlen(header_text) * 8) / 2, .y = 0 });
	printf_rainbow("%s", header_text);

	set_cursor((struct point_t) { .x = 0, .y = 3 * 8 });
	printf("ID  CPU  LOCKED  NAME\n");

	for (int i = 0; i < screen_size.x / 8 - 1; i++) {
		printf("-");
	}

	printf("\n");


	for (int i = 0; i < count; i++) {
		if (tasks[i] == 0) {
			continue;
		}

		printf("%d", i);
		// add spaces to align the numbers
		for (int j = 0; j < 4 - (int) num_length(i); j++) {
			printf(" ");
		}

		printf("%d", tasks[i]->running_on_cpu);
		// add spaces to align the numbers
		for (int j = 0; j < 5 - (int) num_length(tasks[i]->running_on_cpu); j++) {
			printf(" ");
		}

		printf("%s", tasks[i]->lock ? "YES" : "NO");
		// add spaces to align the numbers
		for (int j = 0; j < 8 - (int) strlen(tasks[i]->lock ? "YES" : "NO"); j++) {
			printf(" ");
		}

		printf("%s\n", tasks[i]->argv[0]);
	}

	set_cursor((struct point_t) { .x = 0, .y = screen_size.y - 32 });
	printf_rainbow("memory used: %d MB / %d MB (free: %d MB, reserved: %d MB)", used_memory_mb, total_memory_mb, free_memory_mb, reserved_memory_mb);
	set_cursor((struct point_t) { .x = 0, .y = screen_size.y - 16 });
	printf_rainbow("FoxT (kill, lock, unlock, exit) > ");
}

int calculate_max_task_count() {
	struct point_t screen_size = get_screen_size();

	return screen_size.y / 16 - 10;
	//                          ^^ numer of lines used for other stuff
}

void restore_terminal() {
	set_color(0);
	clear_screen();
	set_color(old_color);
}