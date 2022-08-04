#include <render.h>
#include <foxe.h>

void render_tui() {
	fox_start_frame(&graphics_buffer_info, true);
    max_length_before_line_wrap = graphics_buffer_info.width / CHAR_WIDTH;
	char edited = '-';
	if (is_edited) {
		edited = '+';
	}

	char buff[512] = { 0 };
	sprintf(buff, "File: %s [%c] Mode: --%s-- Current Line: %d Line: %d,%d Type: [%s]", filename, edited, mode, buffer_ln_idx, ln_cnt, char_cnt, file_type);
	fox_draw_string(&graphics_buffer_info, 0, graphics_buffer_info.height - CHAR_SIZE, buff, 0xffffffff, &font);

	int j = 0;
	int cur_x;
	int cur_y = 0;
	bool cursor_drawn = false;
	bool initial_line_drawn = false;
	int allready_drawn = 0;
	int current_line = 1;
	int space_to_draw;
	sprintf(buff, "%d .", ln_cnt);
	space_to_draw = SPACE_BETWEEN_LINE_NUMBER_TEXT + (strlen(buff) * CHAR_WIDTH);
	cur_x = space_to_draw;

	fox_draw_line(&graphics_buffer_info, space_to_draw - (1 * CHAR_WIDTH), 0, space_to_draw - (1 * CHAR_WIDTH), graphics_buffer_info.height - (2 * CHAR_WIDTH), LINE_NUMBER_COLOR);

	for (int i = 0; i < current_size; i++) {
		if ((ln_cnt - 1 < possible_lines_to_draw || j >= buffer_ln_idx) && allready_drawn <= possible_lines_to_draw) {
			if (!initial_line_drawn) {
				initial_line_drawn = true;
				sprintf(buff, "%d.", current_line);
				fox_draw_string(&graphics_buffer_info, 0, cur_y, buff, LINE_NUMBER_COLOR, &font);
			}

			if (i == buffer_idx) {
				fox_draw_char(&graphics_buffer_info, cur_x, cur_y, '|', CURSOR_COLOR, &font);
				cur_x += CHAR_WIDTH;
				cursor_drawn = true;
			}

			if (input_buffer[i] >= 0x20 && input_buffer[i] <= 0x7E) {
				fox_draw_char(&graphics_buffer_info, cur_x, cur_y, input_buffer[i], 0xffffffff, &font);
			}

			cur_x += CHAR_WIDTH;

			if (input_buffer[i] == '\n') {
				allready_drawn++;
				current_line++;
				cur_x = space_to_draw;
				cur_y += CHAR_SIZE;
				sprintf(buff, "%d.", current_line);
				fox_draw_string(&graphics_buffer_info, 0, cur_y, buff, LINE_NUMBER_COLOR, &font);
			} else if ((cur_x / CHAR_WIDTH) % max_length_before_line_wrap == 0) {
                cur_y += CHAR_SIZE;
				cur_x = space_to_draw;
				allready_drawn++;
            }
		} else {
			if (input_buffer[i] == '\n') {
				current_line++;
				j++;
			}
		}
	}

	if (!cursor_drawn) {
		if (!initial_line_drawn) {
			sprintf(buff, "%d.", current_line);
			fox_draw_string(&graphics_buffer_info, 0, cur_y, buff, LINE_NUMBER_COLOR, &font);
		}
		fox_draw_char(&graphics_buffer_info, cur_x, cur_y, '|', 0x33cccc, &font);
	}

	fox_end_frame(&graphics_buffer_info);
}
