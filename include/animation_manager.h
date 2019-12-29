#ifndef TESTGAME_ANIMATION_MANAGER_H
#define TESTGAME_ANIMATION_MANAGER_H

#include <assets/texture.h>
#include <support/sprite_animation.h>

typedef struct {
	sprite_animation ** idling_animations;
	unsigned int idling_animation_count;
	bool idling;

	sprite_animation ** animations;
	unsigned int animation_count;

	float timer;
	int current_animation;
	bool current_animation_looping;
} animation_manager;

animation_manager* animation_manager_new();
void animation_manager_delete(animation_manager* manager);

unsigned int add_idle_animation(animation_manager* manager, sprite_animation* animation);
unsigned int add_animation(animation_manager* manager, sprite_animation* animation);

void update_time(animation_manager* manager, float time);
void start_idling(animation_manager *manager);
void start_animation(animation_manager* manager, unsigned int index, bool looping);
texture* get_current(animation_manager* manager, bool startIdling);
bool has_animation(animation_manager* manager);

#endif //TESTGAME_ANIMATION_MANAGER_H
