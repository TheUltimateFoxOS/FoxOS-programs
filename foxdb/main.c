#include <stdio.h>
#include <foxdb.h>
#include <assert.h>
#include <string.h>
#include <db_commands.h>

command_t commands[] = {
	{
		.name = "help",
		.help = "exactly that",
		.exec = help
	},
	{
		.name = "keys",
		.help = "List all keys in the db",
		.exec = keys
	},
	{
		.name = "get",
		.help = "Get the value of a key in the db",
		.exec = get
	},
	{
		.name = "remove",
		.help = "Remove a key from the db",
		.exec = remove_
	},
	{
		.name = "new_str",
		.help = "Insert a new string into the db",
		.exec = new_str
	},
	{
		.name = "new_int",
		.help = "Insert a new int into the db",
		.exec = new_int
	},
	{
		.name = "new_bool",
		.help = "Insert a new bool into the db",
		.exec = new_bool
	}
};

void* help(int argc, char** argv, void* db) {
	no_args();

	for (int i = 0; i < sizeof(commands) / sizeof(command_t); i++) {
		printf("%s -> %s\n", commands[i].name, commands[i].help);
	}

	return db;
}

void* load_or_create(char* db_path) {
	FILE* db_file = fopen(db_path, "rb");
	void* db;
	if (!db_file) {
		db_file = fopen(db_path, "wb");
		fclose(db_file);
		db = foxdb_new();
	} else {
		db = foxdb_from_file(db_file);
		fclose(db_file);
	}

	return db;
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		printf("Usage: %s [file] [command] [args...]\n", argv[0]);
		return -1;
	}

	void* db = load_or_create(argv[1]);

	for (int i = 0; i < sizeof(commands) / sizeof(command_t); i++) {
		if (strcmp(argv[2], commands[i].name) == 0) {
			printf("running %s\n", commands[i].name);

			db = commands[i].exec(argc - 2, &argv[2], db);

			goto exit_ok;
		}
	}

	printf("Command %s not found!\n", argv[2]);

	FILE* db_file; // doesent work if i declare it below exit_ok. I HATE YOU GCC
exit_ok:
	db_file = fopen(argv[1], "wb");
	foxdb_to_file(db, db_file);
	fclose(db_file);

	foxdb_del(db);

	return 0;
}