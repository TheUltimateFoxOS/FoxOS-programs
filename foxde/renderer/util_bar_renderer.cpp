#include <renderer/util_bar_renderer.h>

#include <buildin/unix_time.h>
#include <sys/time.h>

#include <config.h>

#include <foxos/fox_graphics.h>

void draw_util_bar() {
    fox_draw_rect(0, 0, screen_size.x, util_bar_height, util_bar_colour);

    fox_draw_string(util_bar_padding, util_bar_padding, "FoxOS DE", 0xFFFFFF, &screen_font);

    long long int unix_time = _time();
	char date[28];
    memset(date, 0, 28);
	unix_time_to_string(unix_time, date);

    fox_draw_string(screen_size.x - (strlen(date) * font_width) - util_bar_padding, util_bar_padding, date, 0xFFFFFF, &screen_font);
}
