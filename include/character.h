#ifndef character_h
#define character_h

#include <corange.h>
#include "support/sprite_animation.h"
#include "animation_manager.h"
#include "health_manager.h"

#define CHARACTER_VELOCITY 5

typedef struct {
	vec2 velocity;
	vec2 position;
	animation_manager* animation_manager;
	bool facing_left;
	bool covering;
	health * char_health;
} character;

character* character_new();
void character_delete(character* c);

void character_walk_left(character*);
void character_walk_right(character*);
void character_cover(character*);
void character_uncover(character* c);

void character_update(character* c);
void character_render(character* c, vec2 camera_position);

#endif