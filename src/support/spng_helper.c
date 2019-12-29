#include "support/spng_helper.h"

#define abortOnFail(errorCode) { \
	int failState = (errorCode); \
  if(failState) { \
    error("spng error: %s", spng_strerror(failState)); \
  } \
}

png_image decode_png(char *filename) {
	png_image image;

	FILE *png = fopen(filename, "rb");
	if (png == NULL) {
		error("error opening input file %s\n", filename);
	}

	fseek(png, 0, SEEK_END);

	long siz_pngbuf = ftell(png);
	rewind(png);

	if (siz_pngbuf < 1) {
		error("Buffer size too small");
	}

	unsigned char *pngbuf = malloc(siz_pngbuf);
	image.png_buffer = pngbuf;

	if (fread(pngbuf, siz_pngbuf, 1, png) != 1) {
		error("fread() failed");
	}

	spng_ctx *ctx = spng_ctx_new(0);
	if (ctx == NULL) {
		error("spng_ctx_new() failed\n");
	}

	abortOnFail(spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE));

	abortOnFail(spng_set_png_buffer(ctx, pngbuf, siz_pngbuf));

	struct spng_ihdr ihdr;
	abortOnFail(spng_get_ihdr(ctx, &ihdr));
	image.ihdr = ihdr;

	size_t out_size;

	abortOnFail(spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &out_size));
	image.channels = 4;
	image.channel_size = 1;

	unsigned char *image_decoded = malloc(out_size);

	abortOnFail(spng_decode_image(ctx, image_decoded, out_size, SPNG_FMT_RGBA8, 0));
	image.image_decoded = image_decoded;

	spng_ctx_free(ctx);

	return image;
}

texture *png_to_texture(png_image image) {
	GLenum format;

	switch (image.channels) {
		case 4:
			format = GL_RGBA;
			break;
		case 3:
			format = GL_RGB;
			break;
		default:
		error("PNG color type %d not supported", image.ihdr.color_type);
	}

	GLenum type;

	switch (image.channel_size) {
		case 1:
			type = GL_UNSIGNED_BYTE;
			break;
		case 2:
			type = GL_UNSIGNED_SHORT;
			break;
		case 4:
			type = GL_UNSIGNED_INT;
			break;
		default:
		error("PNG bit depth %d not supported", image.channel_size);
	}

	texture* t = texture_new();
	t->type = GL_TEXTURE_2D;
	glBindTexture(GL_TEXTURE_2D, texture_handle(t));
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	texture_set_filtering_anisotropic(t);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, image.ihdr.width);
	glTexImage2D(t->type, 0, GL_RGBA, image.ihdr.width, image.ihdr.height, 0, format, type, image.image_decoded);

	return t;
}

void delete_png_image(png_image pngImage) {
	free(pngImage.image_decoded);
	free(pngImage.png_buffer);
}
