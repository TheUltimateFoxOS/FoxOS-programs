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

int main(int argc, char* argv[], char* envp[]) {
	// piping(argc, argv, envp);
	// disk(argc, argv, envp);
	// net(argc, argv, envp);
	// libterm(argc, argv, envp);
	// system_(argc, argv, envp);
	// thread_(argc, argv, envp);
	// tasks(argc, argv, envp);
	graphics(argc, argv, envp);
	// sound(argc, argv, envp);
	// ipc(argc, argv, envp);
}
