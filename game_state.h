#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/bitmap.h>
#include <allegro5/events.h>

#include "input.h"
#include "map.h"
#include "camera.h"

#define ATLAS_TEXTURE_W 3 // Number of sprites in a line of the atlas
#define ATLAS_TEXTURE_H 3 // Number of sprites in a column of the atlas

#define TEXTURE_SIZE 100

typedef struct game_state {
	ALLEGRO_DISPLAY *display;
	ALLEGRO_FONT *font;
	ALLEGRO_EVENT_QUEUE *queue;
	ALLEGRO_TIMER *render_timer;
	ALLEGRO_TIMER *update_timer;
	ALLEGRO_BITMAP *texture_atlas;
	ALLEGRO_BITMAP *textures[NUM_BLOCKS];

	int should_quit;

	camera cam;
	map curr_map;
} game_state;

void init_game(game_state *game, int width, int height, float zoom, char *level_path);
void destroy_game(game_state *game);

void render_game(game_state *game);
void update_game(input_controller *c, game_state *game);
void update_physics(game_state *game);

#endif
