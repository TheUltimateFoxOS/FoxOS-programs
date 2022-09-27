#include <db_commands.h>
#include <foxdb.h>
#include <string.h>

void* append(int argc, char** argv, void* db) {
	x_or_more_args(2);

	foxdb_str_t* old  = foxdb_get_str(db, argv[1]);

	char str[512] = { 0 };
	strcat(str, old->val);
	for (int i = 2; i < argc; i++) {
		strcat(str, argv[i]);
		if (i != argc - 1) {
			strcat(str, " ");
		}
	}

	foxdb_str_t* e = foxdb_str(argv[1], str);

	db = foxdb_update(db, (foxdb_entry_t*) e);

	foxdb_del_entry((foxdb_entry_t*) e);
	foxdb_del_entry((foxdb_entry_t*) old);

	printf("Successfully appended to %s!\n", argv[1]);

	return db;
}