#include <image_reader.h>

#define MAGIC 0xc0ffebabe

typedef struct {
	uint64_t magic; // 0xc0ffebabe
	uint64_t width;
	uint64_t height;
	uint32_t pixels[];
} __attribute__((packed)) fpic_image_t;

bool is_fpic(uint8_t* fpic_data) {
	return ((fpic_image_t*) fpic_data)->magic == MAGIC;
}

int get_fpic_width(uint8_t* fpic_data) {
	return ((fpic_image_t*) fpic_data)->width;
}

int get_fpic_height(uint8_t* fpic_data) {
	return ((fpic_image_t*) fpic_data)->height;
}

uint32_t get_fpic_pixel(uint8_t* fpic_data, int x, int y) {
	fpic_image_t* image = (fpic_image_t*) fpic_data;
	return image->pixels[y * image->width + x];
}

image_reader_t fpic_reader = {
	.is_format = is_fpic,
	.get_width = get_fpic_width,
	.get_height = get_fpic_height,
	.get_pix = get_fpic_pixel
};