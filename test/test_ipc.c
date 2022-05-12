#include <test_ipc.h>

#include <stdbool.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <stdio.h>

void ipc_message(int func, void* data) {
	switch (func) {
		case 0:
			printf("Hello from ipc_message!\n");
			break;
		case 1:
			printf("ipc message: %s\n", (char*) data);
			break;

		default:
			printf("ipc message: unknown function %d\n", func);
			break;
	}
}

int ipc(int argc, char* argv[], char* envp[]) {
	int hid = ipc_register("test", ipc_message);

	printf("hid: %d\n", hid);
	system("ipc_test");

	ipc_unregister(hid);
}