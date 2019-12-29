#include <corange.h>
#include <spng.h>
#include <support/sprite_animation.h>

#include "support/spng_helper.h"

#include "support/sprite_sheet_support.h"

unsigned int strtoui(char* numberRaw) {
	long number = strtol(numberRaw, NULL, 10);
	if(number < 0) {
		error("Sprite size < 0");
	}
	if(number > 1024) {
		error("Sprite too big");
	}
	return (unsigned int) number;
}

sprite_animation* sprite_load_file(char *filename) {
	char spriteWidthRaw[50], spriteHeightRaw[50];
	sscanf(filename, "%*[^.] . %[^x] x %[^.] %*s", spriteWidthRaw, spriteHeightRaw);

	unsigned int px_sprite_width = strtoui(spriteWidthRaw);
	unsigned int sprite_height = strtoui(spriteHeightRaw);

	png_image image = decode_png(filename);

	int amount_sprites = image.ihdr.width / px_sprite_width;
	unsigned char** cut_sprites = malloc(amount_sprites * sizeof(unsigned char*));

	unsigned int pixel_size = image.channels * image.channel_size;

	for(int i = 0; i < amount_sprites; i++) {
		cut_sprites[i] = malloc(px_sprite_width * sprite_height * pixel_size);
	}

	for(int px_y = 0; px_y < image.ihdr.height; px_y++) {
		for(int px_x = 0; px_x < image.ihdr.width ; px_x += px_sprite_width) {

			int current_sprite = px_x / px_sprite_width; //integer division tructates towards 0
			int px_sprite_x = px_x - (px_sprite_width * current_sprite);
			int px_sprite_y = px_y;

			size_t chunk_start_cut_image = (px_sprite_y * px_sprite_width + px_sprite_x) * pixel_size;
			size_t chunk_start = (px_y * image.ihdr.width + px_x) * pixel_size;

				memmove(cut_sprites[current_sprite] + chunk_start_cut_image,
				image.image_decoded + chunk_start,
				        px_sprite_width * pixel_size);
		}
	}

	sprite_animation* a = sprite_animation_new();

	for(int i = 0; i < amount_sprites; i++) {
		png_image cut_image = image;
		struct spng_ihdr cut_image_ihdr = image.ihdr;
		cut_image_ihdr.height = sprite_height;
		cut_image_ihdr.width = px_sprite_width;
		cut_image.ihdr = cut_image_ihdr;
		cut_image.image_decoded = cut_sprites[i];

		texture* t = png_to_texture(cut_image);

		sprite_animation_add_frame(a, t);

		free(cut_sprites[i]);
	}

	delete_png_image(image);
	free(cut_sprites);

	return a;
}
