#include <renderer/task_bar_renderer.h>

#include <foxos/fox_graphics.h>

#include <config.h>

int icon_offset;

void draw_icon() {
    fox_draw_rect_outline(&graphics_buffer_info, task_bar_icon_padding, icon_offset, task_bar_icon_size, task_bar_icon_size, 0xFFFFFF);

    icon_offset += task_bar_icon_size + task_bar_icon_padding;
}

void draw_task_bar() {
    fox_draw_rect(&graphics_buffer_info, 0, 0 + task_bar_offset_y, task_bar_width, graphics_buffer_info.width - task_bar_offset_y, task_bar_colour);

    icon_offset = task_bar_offset_y + task_bar_icon_padding;

    draw_icon();
    draw_icon();
}