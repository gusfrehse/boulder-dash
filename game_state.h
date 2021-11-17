#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/bitmap.h>
#include <allegro5/events.h>

#include "input.h"
#include "map.h"
#include "camera.h"


typedef struct game_state {
	ALLEGRO_DISPLAY *display;
	ALLEGRO_FONT *font;
	ALLEGRO_EVENT_QUEUE *queue;
	ALLEGRO_TIMER *render_timer;
	ALLEGRO_TIMER *update_timer;
	ALLEGRO_BITMAP *texture_atlas;
	ALLEGRO_BITMAP *textures[NUM_BLOCKS];

	int should_quit;
	
	int atlas_width, atlas_height;
	int texture_size;

	camera cam;
	map curr_map;

	int status_bar_height;
	int curr_score;
	int curr_diamonds;
	int curr_lives;
	double level_start_time;
	// TODO: init this
} game_state;

void init_game(game_state *game, int width, int height, float zoom, int atlas_width, int atlas_height, int texture_size, char *level_path);
void destroy_game(game_state *game);

void update_game(input_controller *c, game_state *game);
void update_physics(game_state *game);
void render_status_bar(game_state *game);

#endif
