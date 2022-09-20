#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/dir_at.h>
#include <sys/env.h>
#include <sys/open.h>
#include <sys/close.h>
#include <sys/fs_at.h>
#include <sys/sm32_get_cpu_info.h>
#include <buildin/arg.h>

#define max_modules 512

int main(int argc, char *argv[]) {
	arg(help);
	arg(lsfs);
	arg(lsmod);
	arg(lscpu);
	arg_str(path);

	if (help) {
		printf("Usage: %s [--help] [--lsfs] [--lsmod] [--lscpu] [--path <path>]\n", argv[0]);
		return 0;
	} else if (lscpu) {
		cpu_info_t* cpu_info = (cpu_info_t*) malloc(sizeof(cpu_info_t) * 64);

		int len = sm32_get_cpu_info(cpu_info);

		for (int i = 0; i < len; i++) {
			printf("%d: %s %s @%d\n", cpu_info[i].id, cpu_info[i].manufacture, cpu_info[i].version, cpu_info[i].speed);
		}

		free(cpu_info);
	} else if (lsfs) {
		char out[512];
		memset(out, 0, 512);

		int idx = 0;
		while(fs_at(idx++, out)) {
			printf("%s:\n", out);
			memset(out, 0, 512);
		}
	} else if (lsmod) {
		char* modules[max_modules];
		memset(modules, 0, sizeof(char*) * max_modules);

		env_set2(ENV_GET_LOADED_MODULES, max_modules, modules);

		for (int i = 0; i < max_modules && modules[i] != 0; i++) {
			printf("%s\n", modules[i]);
		}
	} else {
		char ls_path[256];
		memset(ls_path, 0, sizeof(ls_path));

	#ifndef USE_ENV_VAR_ROOT_FS
		strcpy(ls_path, (char*) env(ENV_GET_CWD));
	#else
		strcpy(ls_path, getenv("ROOT_FS"));
	#endif

		if (path != NULL) {
			bool canresolve = resolve_check(path, ls_path, true);
			if (!canresolve) {
				printf("Error: No such file or directory: %s\n", argv[1]);
				return 1;
			}
		}

		if (argc == 2) {
			bool canresolve = resolve_check(argv[1], ls_path, true);
			if (!canresolve) {
				printf("Error: No such file or directory: %s\n", argv[1]);
				return 1;
			}
		}

		int fd = open(ls_path);
		if (fd != -1) {
			printf("Error: You can't list a file\n", ls_path);
			close(fd);
			return 1;
		}

		dir_t dir = dir_at(0, ls_path);
		if (dir.is_none) {
			return 0;
		}

		do {
			printf("%s (%s)\n", dir.name, dir.type == ENTRY_FILE ? "file" : "dir");
			dir = dir_at(dir.idx + 1, ls_path);
		} while (!dir.is_none);
	}

	return 0;
}