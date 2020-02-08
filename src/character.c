#include <support/sprite_animation.h>
#include <level.h>
#include "character.h"

static unsigned int ANIMATION_WALKING;

void load_animation_manager(animation_manager* manager) {
	sprite_animation* idle1 = asset_get(P("./tiles/character_walk.32x64.pngsprite"));
	idle1->frame_time = 2.f / 24.f;

	add_idle_animation(manager, idle1);


	sprite_animation* walk = asset_get(P("./tiles/character_walk.32x64.pngsprite"));
	walk->frame_time = 0.5f;

	ANIMATION_WALKING = add_animation(manager, walk);
}

character* character_new() {
	character* c = malloc(sizeof(character));
	c->position = vec2_zero();
	c->velocity = vec2_zero();
	c->animation_manager = animation_manager_new();
	c->facing_left = false;
	c->covering = false;
	c->char_health = health_new();

	load_animation_manager(c->animation_manager);

	return c;
}

void character_delete(character* c) {
	health_delete(c->char_health);
	animation_manager_delete(c->animation_manager);
	free(c);
}

void character_update(character* c) {
	c->velocity.x = clamp(c->velocity.x, -7.0, 7.0);
	c->position = vec2_add(c->position, c->velocity);

	update_time(c->animation_manager, frame_time());
}

void character_walk_left(character* c) {
	if(!c->covering) {
		c->velocity.x = -1;
		start_animation(c->animation_manager, ANIMATION_WALKING, true);
	}
	c->facing_left = true;
}

void character_walk_right(character* c) {
	if(!c->covering) {
		c->velocity.x = 1;
		start_animation(c->animation_manager, ANIMATION_WALKING, true);
	}
	c->facing_left = false;
}

void character_cover(character* c) {
	c->covering = true;
}

void character_uncover(character *c) {
	c->covering = false;
}

/* Renders a simple quad to the screen */

void character_render(character* c, vec2 camera_position) {

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(camera_position.x - graphics_viewport_width() / 2,
	        camera_position.x + graphics_viewport_width() / 2,
	        -camera_position.y + graphics_viewport_height() / 2,
	        -camera_position.y - graphics_viewport_height() / 2, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	texture* character_tex;
	if (c->covering) {
		character_tex = asset_get(P("./tiles/character_cover.png"));
	} else {
		character_tex = get_current(c->animation_manager, true);
	}
	glBindTexture(GL_TEXTURE_2D, texture_handle(character_tex));

	if (c->facing_left) {

		glBegin(GL_TRIANGLES);
		glTexCoord2f(1, 1);
		glVertex3f(c->position.x, c->position.y + CHAR_SIZE_HEIGHT, 0);
		glTexCoord2f(1, 0);
		glVertex3f(c->position.x, c->position.y, 0);
		glTexCoord2f(0, 0);
		glVertex3f(c->position.x + CHAR_SIZE_WIDTH, c->position.y, 0);

		glTexCoord2f(1, 1);
		glVertex3f(c->position.x, c->position.y + CHAR_SIZE_HEIGHT, 0);
		glTexCoord2f(0, 1);
		glVertex3f(c->position.x + CHAR_SIZE_WIDTH, c->position.y + CHAR_SIZE_HEIGHT, 0);
		glTexCoord2f(0, 0);
		glVertex3f(c->position.x + CHAR_SIZE_WIDTH, c->position.y, 0);
		glEnd();

	} else {

		glBegin(GL_TRIANGLES);
		glTexCoord2f(0, 1);
		glVertex3f(c->position.x, c->position.y + CHAR_SIZE_HEIGHT, 0);
		glTexCoord2f(0, 0);
		glVertex3f(c->position.x, c->position.y, 0);
		glTexCoord2f(1, 0);
		glVertex3f(c->position.x + CHAR_SIZE_WIDTH, c->position.y, 0);

		glTexCoord2f(0, 1);
		glVertex3f(c->position.x, c->position.y + CHAR_SIZE_HEIGHT, 0);
		glTexCoord2f(1, 1);
		glVertex3f(c->position.x + CHAR_SIZE_WIDTH, c->position.y + CHAR_SIZE_HEIGHT, 0);
		glTexCoord2f(1, 0);
		glVertex3f(c->position.x + CHAR_SIZE_WIDTH, c->position.y, 0);
		glEnd();

	}

	glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}