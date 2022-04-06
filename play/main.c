#include <stdio.h>
#include <stdlib.h>
#include <sys/sound_push_note.h>
#include <sys/sound_sync.h>

struct note_t {
	uint16_t length_ms;
	uint16_t note;
};

int main(int argc, char* argv[], char* envp[]) {
	if (argc != 2) {
		printf("Usage: %s <notes file>\n", argv[0]);
		return 1;
	}

	FILE* f = fopen(argv[1], "r");
	if (!f) {
		printf("Failed to open %s\n", argv[1]);
		return 1;
	}

	void* buffer = malloc(f->size);
	fread(buffer, f->size, 1, f);
	fclose(f);

	struct note_t* notes = buffer;

	for (int i = 0; i < f->size / sizeof(struct note_t); i++) {
		sound_push_note(notes[i].note, 0, notes[i].length_ms);
		sound_push_note(0, 0, 250);
	}

	free(buffer);

	printf("Waiting for sync...\n");
	sound_sync();

	return 0;
}