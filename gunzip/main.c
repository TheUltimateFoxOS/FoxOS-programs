#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <tinf.h>
#include <string.h>

static unsigned int read_le32(const unsigned char *p) {
	return ((unsigned int) p[0]) | ((unsigned int) p[1] << 8) | ((unsigned int) p[2] << 16) | ((unsigned int) p[3] << 24);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s file\n", argv[0]);
		return 1;
	}

	char output[0xff] = { 0 };
	int len = strlen(argv[1]);

	if (len < 4 || strcmp(argv[1] + len - 3, ".gz") != 0) {
		printf("File %s is not a gzip file!\n", argv[1]);
		abort();
	}
	strcpy(output, argv[1]);
	output[len - 3] = 0;

	printf("decompressing %s -> %s\n", argv[1], output);

	FILE* file = fopen(argv[1], "r");
	if (file == NULL) {
		printf("Error: Could not open file %s\n", argv[1]);
		abort();
	}

	char* buffer = (char*) malloc(file->size);
	fread(buffer, file->size, 1, file);

	tinf_init();

	unsigned int dlen = read_le32(buffer + file->size - 4);
	char* dest = (char*) malloc(dlen);
	unsigned int outlen = dlen;
	int res = tinf_gzip_uncompress(dest, &outlen, buffer, file->size);
	if ((res != TINF_OK) || (outlen != dlen)) {
		printf("decompression failed!\n");
		free(dest);
		free(buffer);
		abort();
	}
	printf("decompressed %d bytes to %d bytes\n", file->size, outlen);
	FILE* out = fopen(output, "w");
	fwrite(dest, outlen, 1, out);
	fclose(out);
	fclose(file);
	free(dest);
	free(buffer);

	return 0;
}