#include <db_commands.h>
#include <foxdb.h>

void* remove_(int argc, char** argv, void* db) {
	x_args(1);

	db = foxdb_remove(db, argv[1]);

	printf("Successfully removed %s!\n", argv[1]);

	return db;
}