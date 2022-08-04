#include <test_arrow_keys.h>

#include <sys/env.h>
#include <stdint.h>
#include <stdio.h>

void arrow_keys_sighandler(uint8_t signum) {
	switch (signum) {
		case SIG_UP_ARROW:
			printf("UP_ARROW\n");
			break;
		case SIG_DOWN_ARROW:
			printf("DOWN_ARROW\n");
			break;
		case SIG_LEFT_ARROW:
			printf("LEFT_ARROW\n");
			break;
		case SIG_RIGHT_ARROW:
			printf("RIGHT_ARROW\n");
			break;
	}
}

int arrow_keys(int argc, char* argv[], char* envp[]) {
	env_set2(ENV_SIGHANDLER, SIG_UP_ARROW, arrow_keys_sighandler);
	env_set2(ENV_SIGHANDLER, SIG_DOWN_ARROW, arrow_keys_sighandler);
	env_set2(ENV_SIGHANDLER, SIG_LEFT_ARROW, arrow_keys_sighandler);
	env_set2(ENV_SIGHANDLER, SIG_RIGHT_ARROW, arrow_keys_sighandler);

	while (getchar() != 'q') {
		// do nothing
	}
}
