#ifndef character_h
#define character_h

#include <corange.h>
#include "support/sprite_animation.h"

#define CHARACTER_VELOCITY 5

typedef struct {
  vec2 velocity;
  vec2 position;
  sprite_animation* sprite_animation_walk;
  float walk_timer;
  bool facing_left;
  bool covering;
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