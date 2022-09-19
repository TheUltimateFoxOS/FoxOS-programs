#include <db_commands.h>
#include <foxdb.h>

void* new_str(int argc, char** argv, void* db) {
	x_or_more_args(2);

	char str[512] = { 0 };
	for (int i = 2; i < argc; i++) {
		strcat(str, argv[i]);
		strcat(str, " ");
	}

	foxdb_str_t* e = foxdb_str(argv[1], str);

	db = foxdb_insert(db, (foxdb_entry_t*) e);

	foxdb_del_entry((foxdb_entry_t*) e);

	printf("Successfully added %s!\n", argv[1]);

	return db;
}