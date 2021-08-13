#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <errno.h>

#include <setjmp.h>
#include <stdlib.h>

#include <sys/resolve_symbol.h>
#include <sys/read.h>
#include <sys/env.h>
#include <sys/sched.h>

#include <buildin/list.h>
#include <buildin/sighandler.h>

jmp_buf buf;

void enumerate(struct list_node_t* node) {
	printf("Node: 0x%x, %d\n", node, node->data);
}

void handle_debug_intr(int signum) {
	printf("Got interrupt %d aka %s!\n", signum, __libc_get_exception_name(signum));
}

int main(int argc, char* argv[], char* envp[]) {
	printf("Hello world!\n");

	//for (int i = 0; i < argc; i++) {
	//	printf("%s\n", argv[i]);
	//}

	env_set2(ENV_SIGHANDLER, 1, handle_debug_intr);
	__asm__ __volatile__("int $1");
	sched();


	printf("task_entry.exit: 0x%x\n", resolve_symbol("task_entry.exit"));
	sched();
	
	char buffer[16];

	printf("Type something: ");

	read(STDIN, buffer, 16);
	buffer[16] = 0;
	sched();

	if (errno == 0xded) {
		printf("Could not read from stdin (already reading from other procces)!\n");
		return -1;
	}

	printf("\nYou just typed: %s!\n", buffer);

	return 0;
}
