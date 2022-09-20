#include <stdio.h>
#include <stdlib.h>
#include <foxdb.h>
#include <foxos/keyboard_helper.h>
#include <sys/dbg.h>

int main() {
	debugf("Loading system configuration...");
	
	SYSDB(sysdb, getenv("ROOT_FS"));
	if (sysdb) {
		foxdb_str_t* keyboard_layout = foxdb_get_str(sysdb, "keyboard_layout");
		foxos_set_keymap(keyboard_layout->val);
		foxdb_del_entry((foxdb_entry_t*) keyboard_layout);

		foxdb_bool_t* keyboard_debug = foxdb_get_bool(sysdb, "keyboard_debug");
		foxos_set_keyboard_debug(keyboard_debug->val);
		foxdb_del_entry((foxdb_entry_t*) keyboard_debug);

		foxdb_del(sysdb);
	} else {
		printf("WARNING: Could not open sysdb (sys.fdb) file.\n");
	}
}