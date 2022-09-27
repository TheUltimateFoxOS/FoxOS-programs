#include <db_commands.h>
#include <foxdb.h>

void* get(int argc, char** argv, void* db) {
	x_args(1);

	foxdb_entry_t* e = foxdb_get(db, argv[1]);
	if (e) {
		switch(e->type) {
			case FOXDB_INT:
				printf("%d\n", ((foxdb_int_t*) e)->val);
				break;

			case FOXDB_BOOL:
				printf("%s\n", ((foxdb_bool_t*) e)->val ? "true" : "false");
				break;
			
			case FOXDB_FLOAT:
				printf("%f\n", ((foxdb_float_t*) e)->val);
				break;

			case FOXDB_STR:
				printf("%s\n", ((foxdb_str_t*) e)->val);
				break;
				
			case FOXDB_BIN:
				{
					foxdb_bin_t* bin = (foxdb_bin_t*) e;
					printf("%ld@%ldB\n", bin->cid, bin->size);
					for (int i = 0; i < bin->size; i++) {
						printf("%2x ", bin->val[i]);
					}
					printf("\n");
				}
				break;

			default:
				printf("Key %s has unknown type %d!\n", argv[1], e->type);
				break;
		}

		foxdb_del_entry(e);
	} else {
		printf("Key %s not found!\n", argv[1]);
	}
	
	return NULL;
}