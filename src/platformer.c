#include <stdio.h>
#include <math.h>

#include <corange.h>
#include <animation_manager.h>
#include <turret.h>

#include "level.h"
#include "character.h"
#include "coin.h"

#include "support/png_support.h"
#include "support/sprite_sheet_support.h"

#include "platformer.h"

/* Some game state variables */
static level* current_level = NULL;
static vec2 camera_position = {0, 0};
static int level_score = 0;
static float level_time = 0;

/* We store all the coin positions here */
enum {
  COIN_COUNT = 45,
};

static vec2 coin_positions[COIN_COUNT] = {
  {16, 23}, {33, 28}, {41, 22}, {20, 19}, {18, 28},
  {36, 20}, {20, 30}, {31, 18}, {45, 23}, {49, 26},
  {25, 18}, {20, 37}, {44, 32}, {66, 20}, {52, 20},
  {63, 11}, {52, 12}, {39, 13}, {27, 11}, {73, 20},
  {65, 29}, {72, 29}, {78, 30}, {78, 20}, {83, 22},
  {87, 22}, {90, 24}, {94, 19}, {99, 18}, {82, 13},
  {79, 14}, {106, 22}, {102, 30}, {100, 35}, {93, 27},
  {88, 34}, {98, 40}, {96, 40}, {94, 40}, {86, 40},
  {81, 37}, {77, 38}, {72, 34}, {65, 38}, {71, 37}
};

static void reset_game() {

  /* Set the starting level to demo.level */
  current_level = asset_get(P("./levels/demo.level"));
  level_score = 0;
  level_time = 0.0;
  
  /* New main character entity */
  character* main_char = entity_get("main_char");
  main_char->position = vec2_mul( vec2_new(20, 20), TILE_SIZE);
  main_char->velocity = vec2_zero();

	turret* turret_0 = entity_get("turret");
	turret_0->position = vec2_mul( vec2_new(19, 20), TILE_SIZE);

  /* We can create multiple entities using a name format string like printf */
  entities_new("coin_id_%i", COIN_COUNT, coin);
  
  /* Get an array of pointers to all coin entities */
  coin* coins[COIN_COUNT];
  entities_get(coins, NULL, coin);
  
  /* Set all the coin initial positions */
  for(int i = 0; i < COIN_COUNT; i++) {
    coins[i]->position = vec2_mul(coin_positions[i], TILE_SIZE);
  }
  
  /* Deactivate victory and new game UI elements */
  ui_button* victory = ui_elem_get("victory");
  ui_button* new_game = ui_elem_get("new_game");
  
  victory->active = false;
  new_game->active = false;
}

void platformer_init() {
  
  /* Set the viewport title */
  graphics_viewport_set_title("Platformer");
  
  /* Register functions for loading/unloading files with the extension .level */
  asset_handler(level, "level", level_load_file, level_delete);

	asset_handler(texture, "png", png_load_file, texture_delete);

	asset_handler(sprite_animation, "pngsprite", sprite_load_file, sprite_animation_delete);

	/* Load Assets */
  folder_load(P("./tiles/"));
  folder_load(P("./backgrounds/"));
  folder_load(P("./sounds/"));
  folder_load(P("./levels/"));
  
  /* Register some handlers for creating and destroying entity types */
  entity_handler(character, character_new, character_delete);
  entity_handler(turret, turret_new, turret_delete);
  entity_handler(coin, coin_new, coin_delete);
  
  /* Create our main character */
  character* main_char = entity_new("main_char", character);

  turret* turret_0 = entity_new("turret", turret);

  /* Add some UI elements */
  ui_button* framerate = ui_elem_new("framerate", ui_button);
  ui_button_move(framerate, vec2_new(10,10));
  ui_button_resize(framerate, vec2_new(30,25));
  ui_button_set_label(framerate, " ");
  ui_button_disable(framerate);
  
  ui_button* score = ui_elem_new("score", ui_button);
  ui_button_move(score, vec2_new(50, 10));
  ui_button_resize(score, vec2_new(120, 25));
  ui_button_set_label(score, "Score 000000");
  ui_button_disable(score);
  
  ui_button* time = ui_elem_new("time", ui_button);
  ui_button_move(time, vec2_new(180, 10));
  ui_button_resize(time, vec2_new(110, 25));
  ui_button_set_label(time, "Time 000000");
  ui_button_disable(time);
  
  ui_button* victory = ui_elem_new("victory", ui_button);
  ui_button_move(victory, vec2_new(365, 200));
  ui_button_resize(victory, vec2_new(70, 25));
  ui_button_set_label(victory, "Victory!");
  ui_button_disable(victory);
  
  ui_button* new_game_but = ui_elem_new("new_game", ui_button);
  ui_button_move(new_game_but, vec2_new(365, 230));
  ui_button_resize(new_game_but, vec2_new(70, 25));
  ui_button_set_label(new_game_but, "New Game");
  
  
  void on_newgame(ui_button* b, void* unused) {
    reset_game();
  }
  
  ui_button_set_onclick(new_game_but, on_newgame);
  
  /* Reset all the game variables */
  reset_game();
  
}

void platformer_event(SDL_Event event) {
	character* main_char = entity_get("main_char");

	switch(event.type){
  case SDL_KEYDOWN:
  	if(event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_SPACE) {
		  if (event.key.keysym.sym == SDLK_a) {
			  character_walk_left(main_char);
		  }
		  if (event.key.keysym.sym == SDLK_d) {
			  character_walk_right(main_char);
		  }

		  if(main_char->velocity.x != 0 && main_char->velocity.y != 0) {
			  main_char->velocity = vec2_mul(vec2_normalize(main_char->velocity), CHARACTER_VELOCITY);
		  }
	  }

		if (event.key.keysym.sym == SDLK_SPACE) {
			character_cover(main_char);
		}
  break;
  
  case SDL_KEYUP:
    if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_a) {
	    main_char->velocity.x = 0;
    }

		if (event.key.keysym.sym == SDLK_SPACE) {
			character_uncover(main_char);
		}
  break;
  }
    
}

static void collision_detection() {
  
  /*
    Collision is fairly simplistic and looks something like this.
    
     @-----@    We check for collision in those points here which
    @       @   are @ signs. If any are colliding with a solid tile
    |       |   then we shift the player so that they are no longer
    @       @   colliding with it. Also invert the velocity.
     @-----@ 
  */
  
  character* main_char = entity_get("main_char");
  
  const float buffer = 4;
  const float bounce = 0;

  vec2 diff_for_bounce() {
	  return vec2_fmod(main_char->position, TILE_SIZE);
  }

  vec2 diff;
  
  /* Bottom Collision */
  
  diff = diff_for_bounce();
  
  vec2 bottom1 = vec2_add(main_char->position, vec2_new(buffer, CHAR_SIZE_HEIGHT));
  vec2 bottom2 = vec2_add(main_char->position, vec2_new(CHAR_SIZE_WIDTH - buffer, CHAR_SIZE_HEIGHT));
  
  bool bottom1_col = tile_has_collision(level_tile_at(current_level, bottom1));
  bool bottom2_col = tile_has_collision(level_tile_at(current_level, bottom2));
  
  if (bottom1_col || bottom2_col) {
    main_char->position = vec2_add(main_char->position, vec2_new(0,-diff.y));
    main_char->velocity.y *= -bounce;
  }
  
  /* Top Collision */

	diff = diff_for_bounce();

  vec2 top1 = vec2_add(main_char->position, vec2_new(buffer, 0));
  vec2 top2 = vec2_add(main_char->position, vec2_new(CHAR_SIZE_WIDTH - buffer, 0));
  
  bool top1_col = tile_has_collision(level_tile_at(current_level, top1));
  bool top2_col = tile_has_collision(level_tile_at(current_level, top2));
  
  if (top1_col || top2_col) {
    main_char->position = vec2_add(main_char->position, vec2_new(0, CHAR_SIZE_HEIGHT - diff.y));
    main_char->velocity.y *= -bounce;
  }
  
  /* Left Collision */

	diff = diff_for_bounce();

  vec2 left1 = vec2_add(main_char->position, vec2_new(0, buffer));
	vec2 left2 = vec2_add(main_char->position, vec2_new(0, CHAR_SIZE_HEIGHT >> 1));
	vec2 left3 = vec2_add(main_char->position, vec2_new(0, CHAR_SIZE_HEIGHT - buffer));

  bool left1_col = tile_has_collision(level_tile_at(current_level, left1));
  bool left2_col = tile_has_collision(level_tile_at(current_level, left2));
  bool left3_col = tile_has_collision(level_tile_at(current_level, left3));

  if (left1_col || left2_col || left3_col) {
    main_char->position = vec2_add(main_char->position, vec2_new(CHAR_SIZE_WIDTH - diff.x,0));
    main_char->velocity.x *= -bounce;
  }
  
  /* Right Collision */

	diff = diff_for_bounce();

  vec2 right1 = vec2_add(main_char->position, vec2_new(CHAR_SIZE_WIDTH, buffer));
  vec2 right2 = vec2_add(main_char->position, vec2_new(CHAR_SIZE_WIDTH, CHAR_SIZE_HEIGHT >> 1));
  vec2 right3 = vec2_add(main_char->position, vec2_new(CHAR_SIZE_WIDTH, CHAR_SIZE_HEIGHT - buffer));

  bool right1_col = tile_has_collision(level_tile_at(current_level, right1));
  bool right2_col = tile_has_collision(level_tile_at(current_level, right2));
  bool right3_col = tile_has_collision(level_tile_at(current_level, right3));

  if (right1_col || right2_col || right3_col) {
    main_char->position = vec2_add(main_char->position, vec2_new(-diff.x,0));
    main_char->velocity.x *= -bounce;
  }
  
}

static void collision_detection_coins() {
  
  /* We simply check if the player intersects with the coins */
  
  character* main_char = entity_get("main_char");
  
  vec2 top_left = vec2_add(main_char->position, vec2_new(-CHAR_SIZE_WIDTH, -CHAR_SIZE_HEIGHT));
  vec2 bottom_right = vec2_add(main_char->position, vec2_new(CHAR_SIZE_WIDTH, CHAR_SIZE_HEIGHT));
  
  /* Again we collect pointers to all the coin type entities */
  int num_coins = 0;
  coin* coins[COIN_COUNT];
  entities_get(coins, &num_coins, coin); 
  
  for(int i = 0; i < num_coins; i++) {
    /* Check if they are within the main char bounding box */
    if ((coins[i]->position.x > top_left.x) &&
        (coins[i]->position.x < bottom_right.x) &&
        (coins[i]->position.y > top_left.y) && 
        (coins[i]->position.y < bottom_right.y)) {
      
      /* Remove them from the entity manager and delete */
      char* coin_name = entity_name(coins[i]);
      entity_delete(coin_name);
      
      /* Play a nice twinkle sound */
      //TODO audio_sound_play(asset_get_as(P("./sounds/coin.wav"), sound), 0);
      
      /* Add some score! */
      level_score += 10;
      
      /* Update the ui text */
      ui_button* score = ui_elem_get("score");
      sprintf(score->label->string, "Score %06i", level_score);
      ui_text_draw(score->label);
    }
  }
  
  
  ui_button* victory = ui_elem_get("victory");
  
  /* if all the coins are gone and the victory rectangle isn't disaplayed then show it */
  if ((entity_type_count(coin) == 0) && (!victory->active)) {
    ui_button* victory = ui_elem_get("victory");
    ui_button* new_game = ui_elem_get("new_game");
    victory->active = true;
    new_game->active = true;
  }
  
}

void platformer_update() {
  
  character* main_char = entity_get("main_char");
  turret* turret_0 = entity_get("turret");

  
  /* Give the player some gravity speed */
  const float gravity = 0.2;
  main_char->velocity.y += gravity;
  
  /* Update moves position based on velocity */
  character_update(main_char);
  turret_update(turret_0);

  /* Collision detection */
	turret_collision_detection(turret_0, current_level);

  collision_detection();
  collision_detection_coins();
  
  /* Camera follows main character */
  camera_position = vec2_new(main_char->position.x, -main_char->position.y);
  
  /* Update the framerate text */
  ui_button* framerate = ui_elem_get("framerate");
  ui_button_set_label(framerate, frame_rate_string());
  
  /* Update the time text */
  ui_button* victory = ui_elem_get("victory");
  if (!victory->active) {
    level_time += frame_time();
    ui_button* time = ui_elem_get("time");
    sprintf(time->label->string, "Time %06i", (int)level_time);
      ui_text_draw(time->label);
  }
  
}

void platformer_render() {
  
  /* Clear the screen to a single color */
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  
  level_render_background(current_level);

	turrer_render(entity_get_as("turret", turret), camera_position);

	character_render(entity_get_as("main_char", character), camera_position);

  /* Get pointers to all the coins for rendering */
  coin* coins[COIN_COUNT];
  int num_coins = 0;
  entities_get(coins, &num_coins, coin); 
  
  for(int i = 0; i < num_coins; i++) {
    coin_render(coins[i], camera_position);
  }
  
  level_render_tiles(current_level, camera_position);
}

void platformer_finish() {
  /* Entity and asset managers will automatically delete any remaining objects. */
}

int main(int argc, char **argv) {
  
  #ifdef _WIN32
    FILE* ctt = fopen("CON", "w" );
    FILE* fout = freopen( "CON", "w", stdout );
    FILE* ferr = freopen( "CON", "w", stderr );
  #endif
  
  /* Init Corange, pointing to the assets_core folder */
  corange_init("assets_core");
  graphics_viewport_set_title("Birdy");
  graphics_viewport_set_size(800, 600);

  platformer_init();
  
  /* Set the game running, create SDL_Event struct to monitor events */
  bool running = 1;
  SDL_Event event;
  
  while(running) {
    
    /* Frame functions used to monitor frame times, FPS and other */
    frame_begin();
    
    while(SDL_PollEvent(&event)) {

      switch(event.type){
      case SDL_KEYUP:
        /* Exit on ESCAPE and Screenshot on print screen */
        if (event.key.keysym.sym == SDLK_ESCAPE) { running = false; }
        if (event.key.keysym.sym == SDLK_PRINTSCREEN) { graphics_viewport_screenshot(); }
        break;
      case SDL_QUIT:
        /* A quitting event such as pressing cross in top right corner */
        running = false;
        break;
      }
      
      /* Also send this event off to the game and ui */
      platformer_event(event);
      ui_event(event);
    }
    
    platformer_update();
    ui_update();
    
    platformer_render();
    ui_render();
    
    /* Flip the Screen Buffer. We've finished with this frame. */
    graphics_swap(); 
    
    /* This allows us to fix the framerate to 60 fps, even on my laptop with vsync broken */
    frame_end_at_rate(60.f);
  }
  
  platformer_finish();
  
  /* Corange will unload remaining assets and delete any remaining entities */
  corange_finish();
  
  return 0;
}
