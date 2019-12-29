#include <sprite_animation.h>
#include <level.h>
#include "character.h"

character* character_new() {
  character* c = malloc(sizeof(character));
  c->position = vec2_zero();
  c->velocity = vec2_zero();
  c->sprite_animation_walk = asset_get(P("./tiles/character_walk.32x64.pngsprite"));
  c->facing_left = false;
  c->covering = false;
	c->sprite_animation_walk->frame_time = 20.f / 60.f;
	c->walk_timer = sprite_animation_duration(c->sprite_animation_walk);


  return c;
}

void character_delete(character* c) {
  free(c);
}

void character_update(character* c) {
  c->velocity.x = clamp(c->velocity.x, -7.0, 7.0);
  c->position = vec2_add(c->position, c->velocity);
  
  if (c->walk_timer < sprite_animation_duration(c->sprite_animation_walk)) {
    c->walk_timer += frame_time();
  }
}

void character_walk_left(character* c) {
	c->velocity.x = -1;
	c->walk_timer = 0;
	c->facing_left = true;
}

void character_walk_right(character* c) {
	c->velocity.x = 1;
	c->walk_timer = 0;
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
	if(c->walk_timer <  sprite_animation_duration(c->sprite_animation_walk)) {
		character_tex = sprite_animation_sample(c->sprite_animation_walk, c->walk_timer);
	} else if (c->covering) {
		character_tex = asset_get(P("./tiles/character_covering.dds"));
	} else {
		character_tex = asset_get(P("./tiles/character.png"));
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