#include <corange.h>

#include <turret.h>

#include "level.h"
#include <malloc.h>
#include <character.h>
#include <support/extra_math.h>

static int TURRET_SIZE_HEIGHT = 32;
static int TURRET_SIZE_WIDTH = 32;
static int TURRET_SHOT_SIZE_HEIGHT = 8;
static int TURRET_SHOT_SIZE_WIDTH = 8;
static int TURRET_SHOT_VELOCITY = 1 * 60;
static int TURRET_CANNON_SIZE_HEIGHT = 8;
static int TURRET_CANNON_SIZE_WIDTH = 8;

turret *turret_new() {
	turret* t = malloc(sizeof(turret));
	t->shots = list_new();
	t->position = vec2_zero();
	t->last_shot_time = 0;
	t->cannon_angle = 0;
	return t;
}

void turret_delete(turret *t) {
	list_delete(t->shots);
	free(t);
}

void turret_update(turret *t) {
	t->last_shot_time += frame_time();

	for (int i = 0; i < t->shots->num_items; ++i) {
		turret_shot* shot = list_get(t->shots, i);
		shot->position = vec2_add(shot->position, vec2_mul(shot->direction, TURRET_SHOT_VELOCITY * frame_time()));
	}

	if(unlikely(t->last_shot_time > 2)) {
		character* main_char = entity_get("main_char");

		turret_fire(t, main_char->position);
	}

	character* main_char = entity_get("main_char");

	turret_point(t, main_char->position);
}

void turret_collision_detection(turret* t, level* current_level) {
	for (int i = 0; i < t->shots->num_items; ++i) {
		turret_shot_collision_detection(t->shots, i, current_level);
	}
}

static void turret_shot_collision_detection(list* shots, int index, level* current_level) {
	const turret_shot* s = list_get(shots, index);

	const float buffer = 4;

	/* Bottom Collision */
	vec2 bottom1 = vec2_add(s->position, vec2_new(buffer, TURRET_SHOT_SIZE_HEIGHT));
	vec2 bottom2 = vec2_add(s->position, vec2_new(TURRET_SHOT_SIZE_WIDTH - buffer, TURRET_SHOT_SIZE_HEIGHT));

	bool bottom1_col = tile_has_collision(level_tile_at(current_level, bottom1));
	bool bottom2_col = tile_has_collision(level_tile_at(current_level, bottom2));

	if (bottom1_col || bottom2_col) {
		list_pop_at(shots, index);
	}

	/* Top Collision */
	vec2 top1 = vec2_add(s->position, vec2_new(buffer, 0));
	vec2 top2 = vec2_add(s->position, vec2_new(TURRET_SHOT_SIZE_WIDTH - buffer, 0));

	bool top1_col = tile_has_collision(level_tile_at(current_level, top1));
	bool top2_col = tile_has_collision(level_tile_at(current_level, top2));

	if (top1_col || top2_col) {
		list_pop_at(shots, index);
	}

	/* Left Collision */
	vec2 left1 = vec2_add(s->position, vec2_new(0, buffer));
	vec2 left2 = vec2_add(s->position, vec2_new(0, TURRET_SHOT_SIZE_HEIGHT >> 1));
	vec2 left3 = vec2_add(s->position, vec2_new(0, TURRET_SHOT_SIZE_HEIGHT - buffer));

	bool left1_col = tile_has_collision(level_tile_at(current_level, left1));
	bool left2_col = tile_has_collision(level_tile_at(current_level, left2));
	bool left3_col = tile_has_collision(level_tile_at(current_level, left3));

	if (left1_col || left2_col || left3_col) {
		list_pop_at(shots, index);
	}

	/* Right Collision */
	vec2 right1 = vec2_add(s->position, vec2_new(TURRET_SHOT_SIZE_WIDTH, buffer));
	vec2 right2 = vec2_add(s->position, vec2_new(TURRET_SHOT_SIZE_WIDTH, TURRET_SHOT_SIZE_HEIGHT >> 1));
	vec2 right3 = vec2_add(s->position, vec2_new(TURRET_SHOT_SIZE_WIDTH, TURRET_SHOT_SIZE_HEIGHT - buffer));

	bool right1_col = tile_has_collision(level_tile_at(current_level, right1));
	bool right2_col = tile_has_collision(level_tile_at(current_level, right2));
	bool right3_col = tile_has_collision(level_tile_at(current_level, right3));

	if (right1_col || right2_col || right3_col) {
		list_pop_at(shots, index);
	}

	character* main_char = entity_get("main_char");

	vec2 top_left = main_char->position;
	vec2 bottom_right = vec2_add(main_char->position, vec2_new(CHAR_SIZE_WIDTH, CHAR_SIZE_HEIGHT));

	/* Check if they are within the main char bounding box */
	if ((s->position.x > top_left.x) && (s->position.x < bottom_right.x) &&
	    (s->position.y > top_left.y) && (s->position.y < bottom_right.y)) {

		list_pop_at(shots, index);

		//TODO audio_sound_play(asset_get_as(P("./sounds/coin.wav"), sound), 0);
	}
}

void turret_point(turret *t, vec2 position) {
	vec2 direction = vec2_normalize(vec2_abs(vec2_sub(position, t->position)));

	t->cannon_angle = atan2(direction.y, direction.x) + M_2_PI;
}

void turret_fire(turret *t, vec2 position) {
	t->last_shot_time = 0;

	vec2 direction = vec2_normalize(vec2_abs(vec2_sub(position, t->position)));

	turret_shot* s = malloc(sizeof(turret_shot));
	s->position = t->position;
	s->direction = direction;

	list_push_back(t->shots, s);
}

void turrer_render(turret *t, vec2 camera_position) {

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
	texture* turret_tex;
	turret_tex = asset_get(P("./tiles/turret.png"));
	glBindTexture(GL_TEXTURE_2D, texture_handle(turret_tex));

	glBegin(GL_TRIANGLES);
	glTexCoord2f(1, 1);
	glVertex3f(t->position.x, t->position.y + TURRET_SIZE_HEIGHT, 0);
	glTexCoord2f(1, 0);
	glVertex3f(t->position.x, t->position.y, 0);
	glTexCoord2f(0, 0);
	glVertex3f(t->position.x + TURRET_SIZE_WIDTH, t->position.y, 0);

	glTexCoord2f(1, 1);
	glVertex3f(t->position.x, t->position.y + TURRET_SIZE_HEIGHT, 0);
	glTexCoord2f(0, 1);
	glVertex3f(t->position.x + TURRET_SIZE_WIDTH, t->position.y + TURRET_SIZE_HEIGHT, 0);
	glTexCoord2f(0, 0);
	glVertex3f(t->position.x + TURRET_SIZE_WIDTH, t->position.y, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	for (int i = 0; i < t->shots->num_items; ++i) {
		turret_shot_render(list_get(t->shots, i));
	}

	glEnable(GL_TEXTURE_2D);
	texture* cannon_tex;
	cannon_tex = asset_get(P("./tiles/turret_cannon.png"));
	glBindTexture(GL_TEXTURE_2D, texture_handle(cannon_tex));

	vec2 cannon_position = vec2_add(t->position, vec2_new(TURRET_CANNON_SIZE_WIDTH /2, TURRET_CANNON_SIZE_HEIGHT /2));


	glTranslatef(+cannon_position.x, +cannon_position.y, 0);
	glRotatef(to_degrees(t->cannon_angle), 0, 0, 1.f);
	glTranslatef(-cannon_position.x, -cannon_position.y, 0);

	glBegin(GL_TRIANGLES);
	glTexCoord2f(1, 1);
	glVertex3f(cannon_position.x, cannon_position.y + TURRET_SIZE_HEIGHT, 0);
	glTexCoord2f(1, 0);
	glVertex3f(cannon_position.x, cannon_position.y, 0);
	glTexCoord2f(0, 0);
	glVertex3f(cannon_position.x + TURRET_SIZE_WIDTH, cannon_position.y, 0);

	glTexCoord2f(1, 1);
	glVertex3f(cannon_position.x, cannon_position.y + TURRET_SIZE_HEIGHT, 0);
	glTexCoord2f(0, 1);
	glVertex3f(cannon_position.x + TURRET_SIZE_WIDTH, cannon_position.y + TURRET_SIZE_HEIGHT, 0);
	glTexCoord2f(0, 0);
	glVertex3f(cannon_position.x + TURRET_SIZE_WIDTH, cannon_position.y, 0);
	glEnd();

	glBegin (GL_POINTS);
	glVertex3f (cannon_position.x, cannon_position.y, 0);
	glEnd ();

	glDisable(GL_TEXTURE_2D);

	glDisable(GL_BLEND);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}

void turret_shot_render(turret_shot* shot) {
	glEnable(GL_TEXTURE_2D);
	texture* shot_tex;
	shot_tex = asset_get(P("./tiles/turret_shot.png"));
	glBindTexture(GL_TEXTURE_2D, texture_handle(shot_tex));

	glBegin(GL_TRIANGLES);
	glTexCoord2f(1, 1);
	glVertex3f(shot->position.x, shot->position.y + TURRET_SHOT_SIZE_HEIGHT, 0);
	glTexCoord2f(1, 0);
	glVertex3f(shot->position.x, shot->position.y, 0);
	glTexCoord2f(0, 0);
	glVertex3f(shot->position.x + TURRET_SHOT_SIZE_WIDTH, shot->position.y, 0);

	glTexCoord2f(1, 1);
	glVertex3f(shot->position.x, shot->position.y + TURRET_SHOT_SIZE_HEIGHT, 0);
	glTexCoord2f(0, 1);
	glVertex3f(shot->position.x + TURRET_SHOT_SIZE_WIDTH, shot->position.y + TURRET_SHOT_SIZE_HEIGHT, 0);
	glTexCoord2f(0, 0);
	glVertex3f(shot->position.x + TURRET_SHOT_SIZE_WIDTH, shot->position.y, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}
