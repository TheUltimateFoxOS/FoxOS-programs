#include <stdio.h>
#include <string.h>
#include <sys/fs_at.h>

int main(int argc, char* argv[], char* envp[]) {
	char out[512];
	memset(out, 0, 512);

	int idx = 0;
	while(fs_at(idx++, out)) {
		printf("%s:\\\n", out);
		memset(out, 0, 512);
	}

    return 0;
}
