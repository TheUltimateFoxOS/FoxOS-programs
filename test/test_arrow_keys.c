#include <test_arrow_keys.h>

#include <sys/env.h>
#include <stdint.h>
#include <stdio.h>

void arrow_keys_sighandler(uint8_t signum) {
	special_keys_down_t* keys = (special_keys_down_t*) env(ENV_GET_SPECIAL_KEYS);

	if (keys->up_arrow) {
		printf("up arrow\n");
	} else if (keys->down_arrow) {
		printf("down arrow\n");
	} else if (keys->left_arrow) {
		printf("left arrow\n");
	} else if (keys->right_arrow) {
		printf("right arrow\n");
	}
}

int arrow_keys(int argc, char* argv[], char* envp[]) {
	env_set2(ENV_SIGHANDLER, SIG_SPECIAL_KEY_DOWN, arrow_keys_sighandler);

	while (getchar() != 'q') {
		// do nothing
	}
}
