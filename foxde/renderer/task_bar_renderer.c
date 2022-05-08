#include <renderer/task_bar_renderer.h>
#include <config.h>

#include <fox_graphics.h>

int icon_offset;

void draw_icon() {
    fox_draw_rect_outline(task_bar_icon_padding, icon_offset, task_bar_icon_size, task_bar_icon_size, 0xFFFFFF);

    icon_offset += task_bar_icon_size + task_bar_icon_padding;
}

void draw_task_bar() {
    fox_draw_rect(0, 0 + task_bar_offset_y, task_bar_width, screen_size.y - task_bar_offset_y, task_bar_colour);

    icon_offset = task_bar_offset_y + task_bar_icon_padding;

    draw_icon();
    draw_icon();
}