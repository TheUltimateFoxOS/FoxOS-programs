#include <test_sound.h>
#include <sys/sound_push_note.h>
#include <sys/sound_sync.h>
#include <sys/sound_get_channel_count.h>
#include <stdio.h>
#include <stdbool.h>

#define note_4(n) sound_push_note(ENCODE_NOTE(5, n), 0, 500); sound_push_note(0, 0, 250)
#define note_8(n) sound_push_note(ENCODE_NOTE(5, n), 0, 300); sound_push_note(0, 0, 250)

int sound(int argc, char* argv[], char* envp[]) {
	printf("Got %d channels\n", sound_get_channel_count());

	while (true) {
		note_4(NOTE_C);
		note_4(NOTE_D);
		note_4(NOTE_E);
		note_4(NOTE_F);
		note_4(NOTE_G);
		note_4(NOTE_G);
		note_8(NOTE_A);
		note_8(NOTE_A);
		note_8(NOTE_A);
		note_8(NOTE_A);
		note_4(NOTE_G);
		note_8(NOTE_A);
		note_8(NOTE_A);
		note_8(NOTE_A);
		note_8(NOTE_A);
		note_4(NOTE_G);
		note_8(NOTE_F);
		note_8(NOTE_F);
		note_8(NOTE_F);
		note_8(NOTE_F);
		note_4(NOTE_E);
		note_4(NOTE_E);
		note_8(NOTE_G);
		note_8(NOTE_G);
		note_8(NOTE_G);
		note_8(NOTE_G);
		note_4(NOTE_C);


		printf("Waiting for sync...\n");
		sound_sync();
	}
}