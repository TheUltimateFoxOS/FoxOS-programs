#include <keyboard_helper.h>

#include <sys/write.h>
#include <sys/open.h>
#include <sys/close.h>
#include <string.h>

void set_keymap(char* id) {
	char change_keymap_cmd[] = {
		0x1, // opcode change keymap
		0,  // keymap id
		0,  // keymap id
		0,  // keymap id
		0,  // keymap id
		0,  // keymap id
		0,  // keymap id
		0,  // keymap id
		0,  // keymap id
		0,  // keymap id
		0  // keymap id
	};

	strcpy(change_keymap_cmd + 1, id);

	int fd = open("dev:ps2_keyboard");
	write(fd, change_keymap_cmd, sizeof(change_keymap_cmd), 0);
	close(fd);
}

void set_keyboard_debug(bool debug) {
	char change_keyboard_debug_cmd[] = {
		0x2, // opcode change keyboard debug
		debug, // debug
	};

	int fd = open("dev:ps2_keyboard");
	write(fd, change_keyboard_debug_cmd, sizeof(change_keyboard_debug_cmd), 0);
	close(fd);
}