#ifndef TESTGAME_SPNG_HELPER_H
#define TESTGAME_SPNG_HELPER_H

#include <spng.h>
#include <corange.h>

typedef struct {
	unsigned int channels;
	size_t channel_size;
	struct spng_ihdr ihdr;
	unsigned char *image_decoded;
	unsigned char *png_buffer;
} png_image;

typedef union {
	struct spng_plte_entry rgba;
	uint32_t pixel;
} pixel32;

png_image decode_png(char* filename);
void delete_png_image(png_image pngImage);
texture* png_to_texture(png_image image);

void abortOnFail(int error);

#endif //TESTGAME_SPNG_HELPER_H
