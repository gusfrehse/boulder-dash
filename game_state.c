#include "game_state.h"

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/altime.h>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/drawing.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/timer.h>

#include <stdio.h>
#include <string.h>

#include "input.h"
#include "map.h"
#include "camera.h"
#include "score.h"
#include "texture.h"

static ALLEGRO_SAMPLE* load_sample(char *path);
static void init_allegro(int width, int height, game_state* game);
static void load_map(game_state *game, char* path);
static void load_samples(game_state* game);
static void move_rockford(int x_amount, int y_amount, game_state *game);
static void reset_map(game_state *game);

void init_game(game_state *game, int width, int height, float zoom,
				 char *atlas_path, char *level_path, char *score_path) {

	init_allegro(width, height, game);
	fprintf(stderr, "Finished loading Allegro.\n");
	load_map(game, level_path);
	fprintf(stderr, "Finished loading map.\n");
	load_texture_system(&game->texture_system, atlas_path);
	fprintf(stderr, "Finished loading texture system.\n");
	load_samples(game);
	fprintf(stderr, "Finished loading resources.\n");

	game->status = IN_GAME;
	game->status_bar_height = 3 * al_get_font_line_height(game->font);
	game->score_path = score_path;
	game->cam = create_camera(0, 0, width, height, 0, game->status_bar_height, zoom);

	start_level(game);
	fprintf(stderr, "Finished initiazing\n");
}

void start_level(game_state *game) {
	game->curr_score.score = 0;
	strcpy(game->curr_score.name, "unknown");

	game->curr_diamonds = 0;
	game->curr_lives = 3;
	game->level_start_time = al_get_time();
}

static void init_allegro(int width, int height, game_state* game) {
	// TODO: These should be moved somewhere else.
	al_init();
	al_install_keyboard();
	al_install_audio();
	al_init_image_addon();
	al_init_font_addon();
	al_init_primitives_addon();
	al_init_acodec_addon();

	game->display = al_create_display(width, height);
	game->font = al_create_builtin_font();
	game->queue = al_create_event_queue();
	game->realtime_timer = al_create_timer(1.0 / 60.0);
	game->oldschool_timer = al_create_timer(1.0 / 10.0);

	al_register_event_source(game->queue, al_get_keyboard_event_source());
	al_register_event_source(game->queue, al_get_display_event_source(game->display));
	al_register_event_source(game->queue, al_get_timer_event_source(game->oldschool_timer));
	al_register_event_source(game->queue, al_get_timer_event_source(game->realtime_timer));
}

static void load_map(game_state *game, char *path) {
	FILE *map1 = fopen(path, "r");

	if (!map1) {
	fprintf(stderr, "ERROR: Could not open map file '%s'\n", path);
		exit(1);
	}

	game->clean_map = map_from_file(map1);

	copy_map(&game->curr_map, &game->clean_map); // sets curr_map

	fclose(map1);
}

static void load_samples(game_state* game) {
	int num_samples = 5;
	if (!al_reserve_samples(num_samples)) {
		fprintf(stderr, "ERROR: Could not reserve samples\n");
		exit(1);
	}
	game->samples[FALLING] = load_sample("./resources/falling.wav");
	game->samples[DIAMOND_PICKUP] = load_sample("./resources/diamond_pickup.wav");
	game->samples[DIGGING] = load_sample("./resources/digging.wav");
	game->samples[DEATH] = load_sample("./resources/death.wav");
	game->samples[SELECTION] = load_sample("./resources/selection.wav");
}

static ALLEGRO_SAMPLE* load_sample(char *path) {
	ALLEGRO_SAMPLE *sample = al_load_sample(path);
	if (!sample) {
		fprintf(stderr, "ERROR: Could not load sample '%s'.\n", path);
		exit(1);
	}

	fprintf(stderr, "Loaded sample '%s'.\n", path);

	return sample;
}

void destroy_game(game_state *game) {
	free(game->curr_map.board);
	al_destroy_font(game->font);
	al_destroy_display(game->display);
	al_destroy_timer(game->realtime_timer);
	al_destroy_timer(game->oldschool_timer);
	al_destroy_event_queue(game->queue);
}

static void kill_rockford(game_state *game) {
	explode_at(game->curr_map.rockford_x, game->curr_map.rockford_y, game->curr_map);
	al_play_sample(game->samples[DEATH], 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);

	reset_map(game);

	if (!--game->curr_lives)  {// if the lives get to zero it's game over.
		game->status = GAME_OVER;
	}
}

static void apply_physics(int x, int y, game_state *game) {
	// If rock over rock -> rock over falls if possible
	// If rock over dirt -> rock stays
	// If rock over air -> rock moves down
	// If rock over rockford -> if rock in already in movement -> rockford dies ->
	// else -> rock stays
	
	// If we already changed current this frame we don't change it again.
	if (get_block_property(x, y, HAS_CHANGED, game->curr_map))
		return;

	if (get_block_property(x, y, DOES_VANISH, game->curr_map)) {

		set_block_at(x, y, AIR, game->curr_map);
		set_block_property(x, y, HAS_CHANGED, game->curr_map);

		return;
	}

	// We only change blocks that have gravity.
	if (!get_block_property(x, y, HAS_GRAVITY, game->curr_map)) {
		return;
	}

	if (get_block_type(x, y+1, game->curr_map) == ROCKFORD && 
		get_block_property(x, y, IN_MOVEMENT, game->curr_map)) {

		// Rockford is below and the current block is going to kill him :(
		kill_rockford(game);
		return;
	}

	// If below don't collides we can move current down
	if (!get_block_property(x, y + 1, IT_COLLIDES, game->curr_map)) {

		set_block_at(x, y + 1, get_block_type(x, y, game->curr_map), game->curr_map);
		set_block_at(x, y, AIR, game->curr_map);
		set_block_property(x, y + 1, IN_MOVEMENT | HAS_CHANGED, game->curr_map);
		return;
	}

	if (get_block_property(x, y, IT_COLLIDES, game->curr_map)) {

		if (get_block_property(x, y, IN_MOVEMENT, game->curr_map)) {
			// Fell over something so we play a sound.
			al_play_sample(game->samples[FALLING], 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
		}

		// In the case it didn't move
		unset_block_property(x, y, IN_MOVEMENT, game->curr_map);

		// If over unstable block, go to the right or left.
		if (get_block_property(x, y + 1, IS_UNSTABLE, game->curr_map)) {

			if (!get_block_property(x - 1, y, IT_COLLIDES, game->curr_map) &&
				!get_block_property(x - 1, y + 1, IT_COLLIDES, game->curr_map)) {

				// Can move to the left
				set_block_at(x - 1, y, get_block_type(x, y, game->curr_map), game->curr_map);
				set_block_property(x - 1, y, HAS_CHANGED, game->curr_map);
				set_block_at(x, y, AIR, game->curr_map);

				return;

			} else if (!get_block_property(x + 1, y, IT_COLLIDES, game->curr_map) &&
						 !get_block_property(x + 1, y + 1, IT_COLLIDES, game->curr_map)) {

				// Can move to the right
				set_block_at(x + 1, y, get_block_type(x, y, game->curr_map), game->curr_map);
				set_block_property(x + 1, y, HAS_CHANGED, game->curr_map);
				set_block_at(x, y, AIR, game->curr_map);

				return;
			}
		}
	}

	// Don't move otherwise
	unset_block_property(x, y, IN_MOVEMENT, game->curr_map);
}

void update_physics(game_state *game) {
	for (int x = 1; x < game->curr_map.width - 1; x++) {
		for (int y = 1; y < game->curr_map.height - 1; y++) {
			apply_physics(x, y, game);
		}
	}

	for (int x = 1; x < game->curr_map.width - 1; x++) {
		for (int y = 1; y < game->curr_map.height - 1; y++) {
			unset_block_property(x, y, HAS_CHANGED, game->curr_map);
		}
	}
}

void update_game(input_controller *c, game_state *game) {
	if (c->key[ALLEGRO_KEY_ESCAPE])
		game->status = SHOULD_QUIT;

	if (c->key[ALLEGRO_KEY_H] || c->key[ALLEGRO_KEY_F1]) {
		game->status = HELP;
	}

	int h_movement = 0;
	int v_movement = 0;

	h_movement = ((c->key[ALLEGRO_KEY_D]) ? 1 : 0) - ((c->key[ALLEGRO_KEY_A]) ? 1 : 0);
	if (h_movement == 0)
		v_movement = ((c->key[ALLEGRO_KEY_S]) ? 1 : 0) - ((c->key[ALLEGRO_KEY_W]) ? 1 : 0);

	move_rockford(h_movement, v_movement, game);
}

static void move_rockford(int x_amount, int y_amount, game_state *game) {
	int destx = game->curr_map.rockford_x + x_amount;
	int desty = game->curr_map.rockford_y + y_amount;
	
	if (get_block_property(destx, desty, IS_DIGGABLE, game->curr_map) ||
	    !get_block_property(destx, desty, IT_COLLIDES, game->curr_map)) {
	
		// We can dig the block!
		if (get_block_type(destx, desty, game->curr_map) == DIAMOND) {
			al_play_sample(game->samples[DIAMOND_PICKUP], 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
			game->curr_diamonds++;
		} else if(get_block_type(destx, desty, game->curr_map) == DIRT) {
			al_play_sample(game->samples[DIGGING], 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
		}
	
		set_block_at(game->curr_map.rockford_x, game->curr_map.rockford_y, AIR, game->curr_map);
		set_block_at(destx, desty, ROCKFORD, game->curr_map);
		
		game->curr_map.rockford_x = destx;
		game->curr_map.rockford_y = desty;
	
	} else if (get_block_property(destx, desty, IS_PUSHABLE, game->curr_map)) {
		if (destx) {
			// Check if horizontal (we can't push in the vertical)
			// TODO : if next block not collides push the block, maybe add random so it seems difficult to push.
			int pushable_x = destx + x_amount;

			if (get_block_property(pushable_x, desty, IT_COLLIDES, game->curr_map))
				return;

			// Move the thing
			set_block_at(pushable_x, desty, get_block_type(destx, desty, game->curr_map), game->curr_map);
			//set_block_property(pushable_x, desty, HAS_CHANGED, game->curr_map);

			// Move Rockford
			set_block_at(destx, desty, ROCKFORD, game->curr_map);

			set_block_at(game->curr_map.rockford_x, game->curr_map.rockford_y, AIR, game->curr_map);

			game->curr_map.rockford_x = destx;
			game->curr_map.rockford_y = desty;
		}
	}
}

void render_status_bar(game_state *game) {
	char status_str[256];

	al_draw_filled_rectangle(0.0f, 0.0f, game->cam.width, game->status_bar_height, al_map_rgb(100, 100, 100)); 

	sprintf(status_str, "DIAMONDS: %d", game->curr_diamonds);
	al_draw_text(game->font, al_map_rgb(230, 230, 230), game->cam.width * 0.1f, game->status_bar_height * 1.0f / 3.0f, ALLEGRO_ALIGN_LEFT, status_str);

	sprintf(status_str, "SCORE: %d", game->curr_score.score);
	al_draw_text(game->font, al_map_rgb(230, 230, 230), game->cam.width * 0.5f, game->status_bar_height * 1.0f / 3.0f, ALLEGRO_ALIGN_CENTER, status_str);

	sprintf(status_str, "LIVES: %d", game->curr_lives);
	al_draw_text(game->font, al_map_rgb(235, 235, 235), game->cam.width * 0.9f, game->status_bar_height * 1.0f / 3.0f, ALLEGRO_ALIGN_RIGHT, status_str);
}

static void reset_map(game_state *game) {
	block *tmp = game->curr_map.board;

	memcpy(&game->curr_map, &game->clean_map, sizeof(map));

	game->curr_map.board = tmp;

	memcpy(game->curr_map.board, game->clean_map.board, sizeof(block) * game->curr_map.width * game->curr_map.height);
}

