#include <image_reader.h>

bool is_bmp(uint8_t* bmp_data) {
	return bmp_data[0] == 'B' && bmp_data[1] == 'M';
}

int get_bmp_width(uint8_t* bmp_data) {
	return *(int32_t*) (bmp_data + 18);
}

int get_bmp_height(uint8_t* bmp_data) {
	return *(int32_t*) (bmp_data + 22);
}

uint32_t get_bmp_pixel(uint8_t* bmp_data, int x, int y) {
	int width = get_bmp_width(bmp_data);
	int height = get_bmp_height(bmp_data);
	int bpp = *(int16_t*) (bmp_data + 28);
	int row_size = width * bpp / 8;
	int padding = (4 - (row_size % 4)) % 4;
	int offset = 54 + (height - y - 1) * (row_size + padding);
	uint8_t* row = bmp_data + offset;
	uint8_t* pixel = row + x * bpp / 8;
	uint32_t argb = 0;
	if (bpp == 32) {
		argb = *(uint32_t*) pixel;
	} else if (bpp == 24) {
		argb = 0xff000000 | *(uint32_t*) pixel;
	} else if (bpp == 8) {
		argb = 0xff000000 | *pixel << 16 | *pixel << 8 | *pixel;
	}
	return argb;
}

image_reader_t bmp_reader = {
	.is_format = is_bmp,
	.get_width = get_bmp_width,
	.get_height = get_bmp_height,
	.get_pix = get_bmp_pixel
};