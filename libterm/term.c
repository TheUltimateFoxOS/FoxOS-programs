#include <term.h>
#include <cmd.h>

void set_color(uint32_t color) {
	char command[] = {
		0, // opcode
		0, // color1
		0, // color2
		0, // color3
		0, // color4
	};

	uint32_t* color_ptr = (uint32_t* )&command[1];
	*color_ptr = color;

	__libterm_send_command(command, sizeof(command));
}

uint32_t get_color() {
	char command[] = {
		1, // opcode
		0, // color1
		0, // color2
		0, // color3
		0, // color4
	};

	__libterm_send_command(command, sizeof(command));

	uint32_t* color_ptr = (uint32_t* )&command[1];
	return *color_ptr;
}

void set_cursor(struct point_t point) {
	char command[] = {
		2, // opcode
		0, // x1
		0, // x2
		0, // x3
		0, // x4
		0, // x5
		0, // x6
		0, // x7
		0, // x8
		0, // y1
		0, // y2
		0, // y3
		0, // y4
		0, // y5
		0, // y6
		0, // y7
		0, // y8

	};

	uint64_t* x_ptr = (uint64_t* )&command[1];
	uint64_t* y_ptr = (uint64_t* )&command[9];
	*x_ptr = point.x;
	*y_ptr = point.y;

	__libterm_send_command(command, sizeof(command));
}

struct point_t get_cursor() {
	char command[] = {
		3, // opcode
		0, // x1
		0, // x2
		0, // x3
		0, // x4
		0, // x5
		0, // x6
		0, // x7
		0, // x8
		0, // y1
		0, // y2
		0, // y3
		0, // y4
		0, // y5
		0, // y6
		0, // y7
		0, // y8

	};

	__libterm_send_command(command, sizeof(command));

	uint64_t* x_ptr = (uint64_t* )&command[1];
	uint64_t* y_ptr = (uint64_t* )&command[9];
	struct point_t point;
	point.x = *x_ptr;
	point.y = *y_ptr;

	return point;
}

struct point_t get_screen_size() {
	char command[] = {
		4, // opcode
		0, // x1
		0, // x2
		0, // x3
		0, // x4
		0, // x5
		0, // x6
		0, // x7
		0, // x8
		0, // y1
		0, // y2
		0, // y3
		0, // y4
		0, // y5
		0, // y6
		0, // y7
		0, // y8

	};

	__libterm_send_command(command, sizeof(command));

	uint64_t* x_ptr = (uint64_t* )&command[1];
	uint64_t* y_ptr = (uint64_t* )&command[9];
	struct point_t point;
	point.x = *x_ptr;
	point.y = *y_ptr;

	return point;
}

void clear_screen() {
	char command[] = {
		5, // opcode
	};

	__libterm_send_command(command, sizeof(command));

	set_cursor((struct point_t) { 0, 0 });
}

void clear_line() {
	char command[] = {
		6, // opcode
	};

	__libterm_send_command(command, sizeof(command));

	set_cursor((struct point_t) { 0, get_cursor().y });
}

void enable_print_char() {
	char change_keyboard_print_char_cmd[] = {
		0x3, // opcode change print char
		1
	};

	__libterm_send_command_ps2(change_keyboard_print_char_cmd, sizeof(change_keyboard_print_char_cmd));
}

void disable_print_char() {
	char change_keyboard_print_char_cmd[] = {
		0x3, // opcode change print char
		0
	};

	__libterm_send_command_ps2(change_keyboard_print_char_cmd, sizeof(change_keyboard_print_char_cmd));
}