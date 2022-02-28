#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/net.h>
#include <sys/env.h>
#include <sys/spawn.h>
#include <sys/open.h>
#include <sys/close.h>
#include <sys/read.h>
#include <sys/write.h>

// void on_stdout(char* buffer, uint64_t size) {
// 	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
// 	printf("stdout(%x, %d)\n", buffer, size);
// 	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1); // re-enable the pipe
// }

// void on_stderr(char* buffer, uint64_t size) {
// 	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
// 	printf("stderr(%x, %d)\n", buffer, size);
// 	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1); // re-enable the pipe
// }

// void on_stdin(char* buffer, uint64_t size) {
// 	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
// 	printf("stdin(%x, %d)\n", buffer, size);
// 	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1); // re-enable the pipe
// }

struct raw_disk_dev_fs_command {
	uint8_t command;
	uint64_t sector;
	uint32_t sector_count;
	uint64_t buffer;
};

struct gpt_guid {
	uint32_t data1;
	uint16_t data2;
	uint16_t data3;
	uint64_t data4;
} __attribute__((packed));

struct gpt_header {
	char signature[8];
	uint8_t revision[4];
	uint32_t header_size;
	uint32_t crc32;
	uint32_t reserved;
	uint64_t current_lba;
	uint64_t backup_lba;
	uint64_t first_usable_lba;
	uint64_t last_usable_lba;
	struct gpt_guid guid;
	uint64_t partition_entries_startting_lba;
	uint32_t partition_entries_count;
	uint32_t partition_entries_size;
	uint32_t partition_entry_array_crc32;
} __attribute__((packed));

int main(int argc, char* argv[], char* envp[]) {

	// union ip_u ip = dns_resolve("www.gsz-zak.de", 0);
	// printf("%d.%d.%d.%d\n", ip.ip_p[0], ip.ip_p[1], ip.ip_p[2], ip.ip_p[3]);
	// char* http_request = "GET /robots.txt HTTP/1.1\r\nHost: www.gsz-zak.de\r\n\r\n";

	// int socket = connect(ip.ip, 80, TCP_SOCKET);
	// send(socket, http_request, strlen(http_request));

	// char* buffer = malloc(1024);
	// int bytes_read = recv(socket, buffer, 1024);
	// buffer[bytes_read] = 0;
	// printf("%s\n", buffer);
	// free(buffer);

	// disconnect(socket);

	// char* argv_for_new_prog[] = {
	// 	"cat",
	// 	"startup.nsh",
	// 	NULL
	// };

	// task* t = spawn("root:/bin/cat.elf", (char**) argv_for_new_prog, (char**) envp, true);

	// bool task_exit = false;
	// int task_exit_code = 0;
	// t->on_exit = &task_exit;
	// t->exit_code = &task_exit_code;

	// t->stdin_pipe = on_stdin;
	// t->stdout_pipe = on_stdout;
	// t->stderr_pipe = on_stderr;
	// t->pipe_enabled	= true;

	// while (!task_exit) {
	// 	__asm__ __volatile__("pause" :: : "memory");
	// }

	// printf("exit code: %d\n", task_exit_code);

	int fd = open("dev:disk_1");

	char buffer[512];
	struct gpt_header* header = (struct gpt_header*) buffer;

	struct raw_disk_dev_fs_command cmd = {
		.command = 0,
		.sector = 1,
		.sector_count = 1,
		.buffer = (uint64_t) header
	};

	write(fd, &cmd, sizeof(cmd), 0);


	if (memcmp(header->signature, "EFI PART", 8) == 0) {
		printf("EFI partition found\n");
	} else {
		printf("EFI partition not found\n");
	}

	for (int i = 0; i < 512; i++) {
		printf("%c", buffer[i]);
	}

    return 0;
}
