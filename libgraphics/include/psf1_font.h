#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

typedef struct {
	uint8_t magic[2];
	uint8_t mode;
	uint8_t charsize;
} psf1_header_t;

typedef struct {
	psf1_header_t* psf1_Header;
	void* glyph_buffer;
} psf1_font_t;

static inline psf1_font_t fox_load_font(char* path) {
	FILE* f = fopen(path, "rb");
	assert(f != NULL);

	void* buffer = malloc(f->size);
	fread(buffer, f->size, 1, f);
	fclose(f);

	psf1_header_t* header = (psf1_header_t*) buffer;
	assert(header->magic[0] == PSF1_MAGIC0 && header->magic[1] == PSF1_MAGIC1);

	uint64_t glyph_buffer_size = header->charsize * 256;
	if(header->mode == 1) {
		glyph_buffer_size = header->charsize * 512;
	}

	void* glyph_buffer = (buffer + sizeof(psf1_header_t));

	psf1_font_t font = {
		.psf1_Header = header,
		.glyph_buffer = glyph_buffer
	};
	
	return font;
}

static inline void fox_free_font(psf1_font_t* font) {
	free(font->psf1_Header);
}