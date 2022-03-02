#include <test_piping.h>

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include <sys/env.h>
#include <sys/spawn.h>
#include <sys/read.h>

void on_stdout(char* buffer, uint64_t size) {
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
	printf("stdout(%x, %d)\n", buffer, size);
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1); // re-enable the pipe
}

void on_stderr(char* buffer, uint64_t size) {
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
	printf("stderr(%x, %d)\n", buffer, size);
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1); // re-enable the pipe
}

void on_stdin(char* buffer, uint64_t size) {
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
	printf("stdin(%x, %d)\n", buffer, size);
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1); // re-enable the pipe
}

int piping(int argc, char* argv[], char* envp[]) {
	printf("TYPE: ");
	char out[11];
	read(STDIN, out, 10, 0);
	out[10] = '\0';

	printf("\nGot: \"%s\"\n", out);

	//--- Next test ---
	printf("\nNext test:\n");

	char* argv_for_new_prog[] = {
		"cat",
		"startup.nsh",
		NULL
	};

    task_t* task = spawn("root:/bin/cat.elf", (char**) argv_for_new_prog, (char**) envp, true);

	bool task_exit = false;
	int task_exit_code = 0;
	task->on_exit = &task_exit;
	task->exit_code = &task_exit_code;

	task->stdin_pipe = on_stdin;
	task->stdout_pipe = on_stdout;
	task->stderr_pipe = on_stderr;
	task->pipe_enabled	= true;

	while (!task_exit) {
		__asm__ __volatile__("pause" :: : "memory");
	}

	printf("Prog exit code: %d\n", task_exit_code);

    return 0;
}