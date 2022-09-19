#include <db_commands.h>
#include <string.h>
#include <foxdb.h>

void* new_bool(int argc, char** argv, void* db) {
	x_args(2);

	bool b = strcmp(argv[2], "true") == 0;
	foxdb_bool_t* e = foxdb_bool(argv[1], b);

	db = foxdb_insert(db, (foxdb_entry_t*) e);

	foxdb_del_entry((foxdb_entry_t*) e);

	printf("Successfully added %s!\n", argv[1]);

	return db;
}