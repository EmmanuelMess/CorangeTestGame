#ifndef sprite_animation_h
#define sprite_animation_h

#include <corange.h>

typedef struct {
	int texture_count;
	float frame_time;
	texture ** textures;
} sprite_animation;

sprite_animation* sprite_animation_new();
void sprite_animation_delete(sprite_animation* a);
float sprite_animation_duration(sprite_animation* a);

texture* sprite_animation_add_frame(sprite_animation* a, texture* base);
texture* sprite_animation_frame(sprite_animation* a, int i);

texture* sprite_animation_sample(sprite_animation* a, float time);

#endif