#include <db_commands.h>
#include <foxdb.h>

void keys_it(foxdb_entry_t* e, uint64_t of) {
	printf("%s: %d@%ldB\n", e->key, e->type, e->size);
}

void* keys(int argc, char** argv, void* db) {
	no_args();

	foxdb_iterate(db, keys_it);

	return NULL;
}