#include <stdio.h>
#include <stdlib.h>
#include <tools.h>
#include <string.h>
#include <foxdb.h>

int main() {
	printf("Welcome to the FoxOS installer!\n");
	char partition_path[256] = { 0 };
	printf("On witch partition do you want to install FoxOS? (you can get a list of all partitions using the \"ls --lsfs\" command) > ");
	int len = gets(partition_path);

	if (partition_path[len - 1] != ':') {
		printf("Error: Only mountpoints are supported.\n");
		abort();
	}

	char partition_name[256] = { 0 };
	printf("How do you want to name the partition? > ");
	len = gets(partition_name);

	char keyboard_layout[256] = { 0 };
	printf("Which keyboard layout do you want to use? > ");
	len = gets(keyboard_layout);

	create_directory(partition_path, "/BIN");
	create_directory(partition_path, "/EFI");
	create_directory(partition_path, "/EFI/BOOT");
	create_directory(partition_path, "/BOOT");
	create_directory(partition_path, "/BOOT/MODULES");
	create_directory(partition_path, "/FOXCFG");
	create_directory(partition_path, "/RES");

	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/BIN");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/RES");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/BOOT/MODULES");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/EFI/BOOT");

	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "", "startup.nsh");
	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "", "LICENSE");

	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "/BOOT", "foxkrnl.elf");
	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "/FOXCFG", "start.fox");

	write_text_file(partition_path, "FOXCFG/dn.fox", partition_name);

	char* limine_config = (char*) malloc(8192);
	memset(limine_config, 0, 8192);

	strcpy(limine_config, "TIMEOUT 3\n:FoxOS\nKASLR=no\nPROTOCOL=stivale2\nKERNEL_PATH=boot:///BOOT/foxkrnl.elf\n");

	strcat(limine_config, "MODULE_PATH=boot:///BOOT/initrd.saf\nMODULE_STRING=initrd.saf\n");

	strcat(limine_config, "KERNEL_CMDLINE=--initrd=modules:initrd.saf --load_modules=initrd:/ ");
	strcat(limine_config, "--autoexec=");
	strcat(limine_config, partition_name);
	strcat(limine_config, ":/BIN/init.elf ");

	strcat(limine_config, "--keymap_load_path=");
	strcat(limine_config, partition_name);
	strcat(limine_config, ":/RES/\n");

	write_text_file(partition_path, "limine.cfg", limine_config);

	free(limine_config);

	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "", "limine.sys");

	// char* foxos_config = (char*) malloc(8192);
	// memset(foxos_config, 0, 8192);
	// sprintf(foxos_config, "keyboard_layout=%s\nkeyboard_debug=false\n", keyboard_layout);
	// write_text_file(partition_path, "FOXCFG/init.cfg", foxos_config);
	// free(foxos_config);

	void* sysdb = foxdb_new();

	foxdb_str_t* kl = foxdb_str("keyboard_layout", keyboard_layout);
	foxdb_bool_t* kd = foxdb_bool("keyboard_debug", false);

	sysdb = foxdb_insert(sysdb, (foxdb_entry_t*) kl);
	sysdb = foxdb_insert(sysdb, (foxdb_entry_t*) kd);

	foxdb_del_entry((foxdb_entry_t*) kl);
	foxdb_del_entry((foxdb_entry_t*) kd);
	
	char sys_db_path[128] = { 0 };
	sprintf(sys_db_path, "%s/FOXCFG/sys.fdb", partition_path);
	printf("[WRITE] %s\n", sys_db_path);
	FILE* sysdb_file = fopen(sys_db_path, "wb");
	foxdb_to_file(sysdb, sysdb_file);
	fclose(sysdb_file);
	foxdb_del(sysdb);

	char initrd_build_cmd[512] = { 0 };
	sprintf(initrd_build_cmd, "safm %s/BOOT/MODULES/ %s/BOOT/initrd.saf -q", partition_path, partition_path);

	printf("[BUILD] Building initrd now...\n");
	system(initrd_build_cmd);

	printf("\n\n");
	printf("FoxOS has been installed on the partition %s (%s).\n", partition_name, partition_path);
	// printf("If you want to install the bios bootloader you can do that using the following command: \"lminst dev:disk_<raw disk id>\"\nYou can get a list of all available disks by typing \"ls dev:\"\n");

ask_again:
	printf("\nDo you want to run the limine install helper now? (y/n) > ");
	char answer[128] = { 0 };
	gets(answer);

	if (strcmp(answer, "y") == 0) {
		printf("Starting limine install helper...\n");
		char command[256] = { 0 };
		sprintf(command, "terminal %s/RES/lminst.fsh", getenv("ROOT_FS"));
		system(command);
	} else if (strcmp(answer, "n") == 0) {
	} else {
		printf("\n\n");
		printf("Invalid answer.\n");
		goto ask_again;
	}

	printf("\n\nSuccessfully installed FoxOS!\n");

	return 0;
}