#include <corange.h>
#include <spng.h>

#include "support/spng_helper.h"

#include "support/png_support.h"

texture* png_load_file(char* filename) {
	png_image image = decode_png(filename);

	texture* t = png_to_texture(image);

	delete_png_image(image);

	return t;
}

