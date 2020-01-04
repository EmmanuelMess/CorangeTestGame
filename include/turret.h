#ifndef TESTGAME_TURRET_H
#define TESTGAME_TURRET_H

#include "level.h"

typedef struct {
	vec2 direction;
	vec2 position;
} turret_shot;

typedef struct {
	list* shots;
	vec2 position;
	float last_shot_time;
	float cannon_angle;
} turret;

turret* turret_new();
void turret_delete(turret* t);

void turret_update(turret* t);
void turret_collision_detection(turret* t, level* current_level);
void turrer_render(turret* t, vec2 camera_position);
void turret_point(turret* t, vec2 position);
void turret_fire(turret* t, vec2 position);

static void turret_shot_render(turret_shot* s);
static void turret_shot_collision_detection(list* shots, int index, level* current_level);

#endif //TESTGAME_TURRET_H
