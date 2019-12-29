#include "animation_manager.h"

animation_manager* animation_manager_new() {
	animation_manager* manager = malloc(sizeof(animation_manager));

	manager->idling_animations = NULL;
	manager->idling_animation_count = 0;
	manager->idling = false;

	manager->animations = NULL;
	manager->animation_count = 0;

	manager->timer = 0;
	manager->current_animation = -1;
	manager->current_animation_looping = false;

	return manager;
}

void animation_manager_delete(animation_manager* manager) {
	free(manager);
}

unsigned int add_idle_animation(animation_manager *manager, sprite_animation *animation) {
	manager->idling_animation_count++;
	manager->idling_animations = realloc(manager->idling_animations, manager->idling_animation_count * sizeof(sprite_animation*));

	manager->idling_animations[manager->idling_animation_count-1] = animation;

	return manager->idling_animation_count-1;
}

unsigned int add_animation(animation_manager* manager, sprite_animation* animation) {
	manager->animation_count++;
	manager->animations = realloc(manager->animations, manager->animation_count * sizeof(sprite_animation*));

	manager->animations[manager->animation_count-1] = animation;

	return manager->animation_count-1;
}

bool has_animation(animation_manager* manager) {
	return manager->current_animation != -1;
}

void update_time(animation_manager* manager, float deltaTime) {
	if(manager->current_animation == -1) {
		return;
	}

	manager->timer += deltaTime;

	sprite_animation* curr_animation;
	if(manager->idling) {
		curr_animation = manager->idling_animations[manager->current_animation];
	} else {
		curr_animation = manager->animations[manager->current_animation];
	}

	if(manager->timer > sprite_animation_duration(curr_animation)) {
		if(manager->current_animation_looping) {
			manager->timer = 0;
		} else if(manager->idling_animation_count > 0) {
			start_idling(manager);
		} else {
			manager->current_animation = -1;
		}
	}
}

void start_idling(animation_manager *manager) {
	if(manager->idling_animation_count == 0) {
		error("No idling animations");
	}

	manager->idling = true;
	manager->timer = 0;
	manager->current_animation = 0;// TODO be random
	manager->current_animation_looping = false;
}

void start_animation(animation_manager *manager, unsigned int index, bool looping) {
	if(manager->current_animation == index && manager->timer < sprite_animation_duration(manager->animations[index])) {
		return;
	}

	manager->current_animation = index;
	manager->current_animation_looping = looping;
	manager->timer = 0;
}

texture* get_current(animation_manager* manager, bool startIdling) {
	if(manager->current_animation == -1) {
		if(startIdling) {
			start_idling(manager);
		} else {
			error("No animation playing");
		}
	}

	if(manager->idling) {
		return sprite_animation_sample(manager->idling_animations[manager->current_animation], manager->timer);
	} else {
		return sprite_animation_sample(manager->animations[manager->current_animation], manager->timer);
	}
}