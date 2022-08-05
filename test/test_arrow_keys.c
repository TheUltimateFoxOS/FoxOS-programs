#include <test_arrow_keys.h>

#include <sys/env.h>
#include <stdint.h>
#include <stdio.h>

void arrow_keys_sighandler(char* up_or_down) {
	special_keys_down_t* keys = (special_keys_down_t*) env(ENV_GET_SPECIAL_KEYS);

	switch (keys->triggered_by) {
		case up_arrow:
			printf("up arrow %s\n", up_or_down);
			break;
		
		case down_arrow:
			printf("down arrow %s\n", up_or_down);
			break;

		case left_arrow:
			printf("left arrow %s\n", up_or_down);
			break;

		case right_arrow:
			printf("right arrow %s\n", up_or_down);
			break;
	
		default:
			break;
	}
}

void arrow_keys_up_sighandler(uint8_t signum) {
	arrow_keys_sighandler("up");
}

void arrow_keys_down_sighandler(uint8_t signum) {
	arrow_keys_sighandler("down");
}

int arrow_keys(int argc, char* argv[], char* envp[]) {
	env_set2(ENV_SIGHANDLER, SIG_SPECIAL_KEY_UP, arrow_keys_up_sighandler);
	env_set2(ENV_SIGHANDLER, SIG_SPECIAL_KEY_DOWN, arrow_keys_down_sighandler);

	printf("Press q to quit.\n");

	while (getchar() != 'q') {
		// do nothing
	}
}
