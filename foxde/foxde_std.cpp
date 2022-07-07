#include <foxde_std.h>

#include <renderer/terminal_renderer.h>

#include <config.h>

#include <sys/env.h>

#ifdef enable_terminal
void foxde_stdout(char* buffer, uint64_t size) {
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
	print_bg_console(buffer, size);
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1); // re-enable the pipe
}

void foxde_stderr(char* buffer, uint64_t size) {
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 0); // we run here in the task context of the child process, so we disable the pipe while using the stdout/stderr/stdin
	print_bg_console(buffer, size);
	env_set(ENV_PIPE_DISABLE_ENABLE, (void*) 1); // re-enable the pipe
}

#else
void foxde_stdout(char* buffer, uint64_t size) {
	
}

void foxde_stderr(char* buffer, uint64_t size) {
	
}

#endif