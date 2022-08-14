#include <test_piping.h>
#include <test_disk.h>
#include <test_net.h>
#include <test_libterm.h>
#include <test_system.h>
#include <test_tasks.h>
#include <test_thread.h>
#include <test_graphics.h>
#include <test_sound.h>
#include <test_ipc.h>
#include <test_arrow_keys.h>

#include <stdlib.h>
#include <stdio.h>

#include <sys/dbg.h>

int main(int argc, char* argv[], char* envp[]) {
	debugf("Hello world from %s", argv[0]);
	// char buffer[256] = { 0 };
	// long num1 = 0;
	// long num2 = 0;
	// char c;
	// scanf("%s %d  %x %c", buffer, &num1, &num2, &c);
	// printf("\n%s %d %x %c\n", buffer, num1, num2, c);
	// piping(argc, argv, envp);
	// disk(argc, argv, envp);
	// net(argc, argv, envp);
	// libterm(argc, argv, envp);
	// system_(argc, argv, envp);
	// thread_(argc, argv, envp);
	// tasks(argc, argv, envp);
	// graphics(argc, argv, envp);
	// sound(argc, argv, envp);
	// ipc(argc, argv, envp);
	arrow_keys(argc, argv, envp);
}
