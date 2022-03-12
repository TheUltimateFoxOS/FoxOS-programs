#include <test_thread.h>
#include <buildin/thread.h>
#include <stdio.h>

void thread_func() {
	printf("Hello from thread!\n");
	while(1) {
		__asm__ __volatile__("pause" ::: "memory");
	}
}

int thread_(int argc, char* argv[], char* envp[]) {
	task_t* t = create_thread(thread_func);
}