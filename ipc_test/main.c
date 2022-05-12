#include <stdio.h>
#include <sys/ipc.h>
#include <assert.h>

int main(int argc, char* argv[], char* envp[]) {
	printf("Hello from ipc test!\n");

	int hid = ipc_get_hid("test");
	assert(hid != -1);

	printf("hid: %d\n", hid);

	ipc_send_message(hid, 0, NULL);
	ipc_send_message(hid, 1, "Hello from ipc_test!");

	return 0;
}