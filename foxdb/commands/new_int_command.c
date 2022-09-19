#include <db_commands.h>
#include <foxdb.h>

void* new_int(int argc, char** argv, void* db) {
	x_args(2);

	int i = (int) strtol(argv[2], NULL, 10);
	foxdb_int_t* e = foxdb_int(argv[1], i);

	db = foxdb_insert(db, (foxdb_entry_t*) e);

	foxdb_del_entry((foxdb_entry_t*) e);

	printf("Successfully added %s!\n", argv[1]);

	return db;
}