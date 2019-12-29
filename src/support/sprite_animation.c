#include "support/sprite_animation.h"

sprite_animation* sprite_animation_new() {
	sprite_animation* a = malloc(sizeof(sprite_animation));

	a->texture_count = 0;
	a->frame_time = 1.0/30.0;
	a->textures = NULL;

	return a;
}

void sprite_animation_delete(sprite_animation* a) {
	for(int i = 0; i < a->texture_count; i++) {
		texture_delete(a->textures[i]);
	}
	free(a->textures);
	free(a);
}

float sprite_animation_duration(sprite_animation* a) {
	return a->texture_count * a->frame_time;
}

texture* sprite_animation_add_frame(sprite_animation* a, texture* base) {
	a->texture_count++;
	a->textures = realloc(a->textures, sizeof(texture*) * a->texture_count);
	a->textures[a->texture_count-1] = base;

	return base;
}

texture* sprite_animation_frame(sprite_animation* a, int i) {
	i = i < 0 ? 0 : i;
	i = i > (a->texture_count-1) ? (a->texture_count-1) : i;

	return a->textures[i];
}

texture* sprite_animation_sample(sprite_animation* a, float time) {
	if (a->texture_count == 0) {
		error("Animation has no frames!");
		return NULL;
	}

	if (a->texture_count == 1) {
		return a->textures[0];
	}

	return sprite_animation_frame(a, roundf(time / a->frame_time));
}