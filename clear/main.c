#include <foxos/term.h>

int main() {
	uint64_t old_color = get_color();
	set_color(0);

	clear_screen();

	set_color(old_color);
}