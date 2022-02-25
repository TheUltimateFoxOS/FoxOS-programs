#include <stdio.h>
#include <stdlib.h>
#include <tools.h>
#include <string.h>

int main() {
	printf("Welcome to the FoxOS installer!\n");
	char partition_path[256] = { 0 };
	printf("On witch partition do you want to install FoxOS? > ");
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

	create_directory(partition_path, "/bin");
	create_directory(partition_path, "/efi");
	create_directory(partition_path, "/efi/boot");
	create_directory(partition_path, "/boot");
	create_directory(partition_path, "/boot/modules");
	create_directory(partition_path, "/foxcfg");
	create_directory(partition_path, "/res");

	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/bin");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/res");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/boot/modules");
	copy_dir_across_fs(getenv("ROOT_FS"), partition_path, "/efi/boot");

	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "", "startup.nsh");
	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "", "LICENSE");

	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "/boot", "foxkrnl.elf");
	copy_file_across_fs(getenv("ROOT_FS"), partition_path, "/foxcfg", "start.fox");

	write_text_file(partition_path, "foxcfg/dn.fox", partition_name);

	char** modules = malloc(sizeof(char*) * 50);
	char** original_modules = modules;
	list_files(getenv("ROOT_FS"), "/boot/modules", modules);

	char* limine_config = (char*) malloc(8192);
	memset(limine_config, 0, 8192);

	strcpy(limine_config, "TIMEOUT 3\n:FoxOS\nKASLR=no\nPROTOCOL=stivale2\nKERNEL_PATH=boot:///BOOT/foxkrnl.elf\n");

	while (*modules != NULL) {
		strcat(limine_config, "MODULE_PATH=boot:///BOOT/MODULES/");
		strcat(limine_config, *modules);
		strcat(limine_config, "\n");
		strcat(limine_config, "MODULE_STRING=");
		strcat(limine_config, *modules);
		strcat(limine_config, "\n");
		modules++;
	}

	strcat(limine_config, "KERNEL_CMDLINE=");
	modules = original_modules;
	while (*modules != NULL) {
		strcat(limine_config, "--load_module=stivale:");
		strcat(limine_config, *modules);
		strcat(limine_config, " ");
		free(*modules);
		modules++;
	}

	free(original_modules);

	strcat(limine_config, "--autoexec=");
	strcat(limine_config, partition_name);
	strcat(limine_config, ":/bin/init.elf ");

	strcat(limine_config, "--keymap_load_path=");
	strcat(limine_config, partition_name);
	strcat(limine_config, ":/res/\n");

	write_text_file(partition_path, "limine.cfg", limine_config);

	free(limine_config);

	char* foxos_config = (char*) malloc(8192);
	memset(foxos_config, 0, 8192);

	sprintf(foxos_config, "{\n    \"keyboard_layout\": \"%s\"\n}\n", keyboard_layout);

	write_text_file(partition_path, "foxcfg/cfg.fox", foxos_config);

	free(foxos_config);

	printf("\n\n");
	printf("FoxOS has been installed on the partition %s (%s).\n", partition_name, partition_path);

	return 0;
}