#include <foxe.h>

#include <sys/env.h>

#include <foxos/term.h>

#include <input.h>
#include <render.h>
#include <utils.h>


char* copy_buffer;
bool copy_buffer_is_set = false;
int copy_buffer_length;

char* input_buffer;
char* filename;
char* file_type;
FILE* f;
char* mode;
bool is_edited = false;
bool is_in_insert_mode = true;
uint32_t old_color;
int max_length_before_line_wrap;

int current_size = 1;

int possible_lines_to_draw;

unsigned int ln_cnt = 1;
unsigned int char_cnt = 0;

unsigned int buffer_ln_idx = 0;
unsigned int buffer_idx = 0;

psf1_font_t font;

graphics_buffer_info_t graphics_buffer_info;


void render_usage() {
	printf("\tUsage: foxe <file-name>\n\n");

	printf("Cheat sheet\n\t");
	printf("w        -> move up a line\n\t");
	printf("a        -> move left a character\n\t");
	printf("s        -> move down a line\n\t");
	printf("d        -> move right a char\n\n\t");

	printf("W        -> move to top of file\n\t");
	printf("A        -> move to beginning of line\n\t");
	printf("S        -> move to bottom of file\n\t");
	printf("D        -> move to the end of a line\n\n\t");

	printf("r + input -> replace the current character with user input\n\t");

	printf("+        -> write changes to file\n\t");
	printf("-        -> discard changes\n\t");
	printf("q        -> quits the editor (without saving!!!)\n");
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		render_usage();
		return 1;
	}

	graphics_buffer_info = create_screen_buffer();

	char font_path[512] = { 0 };
	sprintf(font_path, "%s/RES/zap-light16.psf", getenv("ROOT_FS"));
	font = fox_load_font(font_path);

	filename = argv[1];
	file_type = "";
	file_type = get_file_extension(argv[1]);
	mode = "INSERT";

	old_color = get_color();
	set_color(0);
	clear_screen();
	set_color(old_color);
	disable_print_char();

	possible_lines_to_draw = graphics_buffer_info.height / CHAR_SIZE - 4;

	// set keyboard input print to false
	// env_set(ENV_NO_PRINT_ON_INPUT, (void*) true);

	env_set2(ENV_SIGHANDLER, SIG_SPECIAL_KEY_DOWN, arrow_keys_sighandler);

	f = fopen(argv[1], "w");
	if (f->size != 0) {
		input_buffer = (char*) malloc(sizeof(char) * f->size);
	}
	current_size = f->size;
	buffer_idx = f->size;
	fread((void*) input_buffer, sizeof(char), f->size * sizeof(char), f);

	for (int i = 0; i < current_size; i++) {
		char_cnt++;
		if (input_buffer[i] == '\n') {
			ln_cnt++;
			buffer_ln_idx++;
			char_cnt--;
		}
	} 


	while (true) {
		render_tui();

		if (listen_input()) {
			break;
		}
	}

	fclose(f);
	set_color(0);
	clear_screen();
	set_color(old_color);
	enable_print_char();

	fox_free_framebuffer(&graphics_buffer_info);
	fox_free_font(&font);
}
