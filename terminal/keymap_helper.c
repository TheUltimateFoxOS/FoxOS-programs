#include <keymap_helper.h>

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