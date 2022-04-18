#include <stdio.h>
#include <fox_graphics.h>

#include <buildin/thread.h>
#include <term.h>

typedef struct {
	uint64_t magic; // 0xc0ffebabe
	uint64_t width;
	uint64_t height;
} __attribute__((packed)) fpic_image_header_t;

bool running = true;
bool save = false;
bool clear = false;
uint32_t current_color = 0xff00ff00; // green

void keyboard_input_task() {
	while (true) {
		switch (getchar()) {
			case 27:
				running = false;
				return;
			
			case 's':
				save = true;
				break;

			case 'r':
				current_color = 0xffff0000; // red
				break;
			
			case 'g':
				current_color = 0xff00ff00; // green
				break;

			case 'b':
				current_color = 0xff0000ff; // blue
				break;
			
			case 'y':
				current_color = 0xffffff00; // yellow
				break;
			
			case 'w':
				current_color = 0xffffffff; // white
				break;
			
			case 'c':
				clear = true;
				break;
		}
	}
}

int main(int argc, char* argv[], char* envp[]) {
	if (argc < 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	disable_print_char();

	char font_path[512] = { 0 };
	sprintf(font_path, "%s/RES/zap-light16.psf", getenv("ROOT_FS"));
	psf1_font_t font = fox_load_font(font_path);

	struct framebuffer_t fb = fb_info();
	void* pixel_field = malloc((fb.width - 4) * (fb.height - 64) * 4);

	create_thread(keyboard_input_task);

	char* status_message = NULL;

	while (running) {
		if (save) {
			FILE* f = fopen(argv[1], "wb");
			assert(f != NULL);

			fpic_image_header_t header = {
				.magic = 0xc0ffebabe,
				.width = fb.width - 4,
				.height = fb.height - 64,
			};

			fwrite(&header, sizeof(header), 1, f);
			fwrite(pixel_field, (fb.width - 4) * (fb.height - 64) * 4, 1, f);
			fclose(f);

			save = false;
			status_message = "Saved";
		}

		if (clear) {
			memset(pixel_field, 0, (fb.width - 4) * (fb.height - 64) * 4);
			clear = false;
			status_message = "Cleared";
		}

		struct mouse_position_t pos = mouse_position();
		int button = mouse_button();

		fox_start_frame(true);

		fox_draw_rect_outline(0, 0, fb.width, fb.height - 62, 0xFFFFFFFF);

		// draw the pixel field
		for (int y = 0; y < fb.height - 64; y++) {
			for (int x = 0; x < fb.width - 4; x++) {
				int offset = (x + y * (fb.width - 4)) * 4;
				uint32_t color = ((uint32_t*) pixel_field)[offset / 4];
				fox_set_px(x + 2, y + 1, color);
			}
		}

		fox_draw_rect(pos.x, pos.y, 10, 10, current_color);

		if (button == MOUSE_BUTTON_LEFT) {
			// if mouse pos is inside the pixel field, draw a pixel
			if (pos.x >= 4 && pos.x < fb.width - 4 && pos.y >= 64 && pos.y < fb.height - 4) {
				int x = pos.x;
				int y = pos.y;
				int offset = (x + y * (fb.width - 4)) * 4;
				((uint32_t*) pixel_field)[offset / 4] = current_color;
			}
		} else if (button == MOUSE_BUTTON_RIGHT) {
			// if mouse pos is inside the pixel field, clear the pixel
			if (pos.x >= 4 && pos.x < fb.width - 4 && pos.y >= 64 && pos.y < fb.height - 4) {
				int x = pos.x;
				int y = pos.y;
				int offset = (x + y * (fb.width - 4)) * 4;
				((uint32_t*) pixel_field)[offset / 4] = 0;
			}
		}

		fox_draw_string(0, fb.height - 60, "Press 's' to save, 'esc' to exit, 'c ' to clear, 'r', 'g', 'b', 'y', 'w' to change color.", 0xFFFFFFFF, &font);
		if (status_message != NULL) {
			fox_draw_string(0, fb.height - 40, status_message, 0xFFFFFFFF, &font);
		}
		fox_end_frame();
	}

	fox_start_frame(true);
	fox_draw_string(0, 0, "Goodbye!", 0xffffffff, &font);
	fox_end_frame();

	set_cursor((struct point_t) { 0, 16 });

	free(pixel_field);
	enable_print_char();

	fox_free_font(&font);
	fox_free_framebuffer();

	return 0;
}