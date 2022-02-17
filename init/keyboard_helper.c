#include <keyboard_helper.h>

#include <sys/write.h>
#include <sys/open.h>
#include <sys/close.h>

void set_keymap(int id) {
	char change_keymap_cmd[] = {
		0x1, // opcode change keymap
		id,  // keymap id
	};

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