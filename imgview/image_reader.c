#include <image_reader.h>

image_reader_t* image_reader_detect(uint8_t* buff) {
	if (bmp_reader.is_format(buff)) {
		return &bmp_reader;
	} else if (fpic_reader.is_format(buff)) {
		return &fpic_reader;
	} else {
		return 0;
	}
}