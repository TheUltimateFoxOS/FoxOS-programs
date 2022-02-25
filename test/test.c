#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/net.h>
#include <sys/env.h>
#include <sys/spawn.h>

void on_stdout(char* buffer, uint64_t size) {
	env_set(ENV_PIPE_DISABLE_ENABLE, 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
	printf("stdout(%x, %d)\n", buffer, size);
	env_set(ENV_PIPE_DISABLE_ENABLE, 1); // re-enable the pipe
}

void on_stderr(char* buffer, uint64_t size) {
	env_set(ENV_PIPE_DISABLE_ENABLE, 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
	printf("stderr(%x, %d)\n", buffer, size);
	env_set(ENV_PIPE_DISABLE_ENABLE, 1); // re-enable the pipe
}

void on_stdin(char* buffer, uint64_t size) {
	env_set(ENV_PIPE_DISABLE_ENABLE, 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
	printf("stdin(%x, %d)\n", buffer, size);
	env_set(ENV_PIPE_DISABLE_ENABLE, 1); // re-enable the pipe
}

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

	char* argv_for_new_prog[] = {
		"cat",
		"startup.nsh",
		NULL
	};

	task* t = spawn("root:/bin/cat.elf", argv_for_new_prog, envp, true);

	bool task_exit = false;
	int task_exit_code = 0;
	t->on_exit = &task_exit;
	t->exit_code = &task_exit_code;

	t->stdin_pipe = on_stdin;
	t->stdout_pipe = on_stdout;
	t->stderr_pipe = on_stderr;
	t->pipe_enabled	= true;

	while (!task_exit) {
		__asm__ __volatile__("pause" :: : "memory");
	}

	printf("exit code: %d\n", task_exit_code);

    return 0;
}
