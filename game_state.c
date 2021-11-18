#include "game_state.h"

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/altime.h>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/drawing.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/timer.h>

#include <stdio.h>
#include <string.h>

#include "input.h"
#include "map.h"
#include "camera.h"

static ALLEGRO_BITMAP *load_texture(char *path);
static ALLEGRO_BITMAP *load_sub_texture(game_state *game, block_type b);
static void init_allegro(int width, int height, game_state* game);
static void load_map(game_state *game);
static void load_textures(int atlas_width, int atlas_height, int texture_size, game_state *game);
static void move_rockford(int x_amount, int y_amount, game_state *game);
static void reset_map(game_state *game);
static void game_over(game_state* game);

void init_game(game_state *game, int width, int height, float zoom,
				 int atlas_width, int atlas_height, int texture_size,
				 char *level_path)
{
	init_allegro(width, height, game);
	load_map(game);
	load_textures(atlas_width, atlas_height, texture_size, game);

	game->status_bar_height = 3 * al_get_font_line_height(game->font);
	game->curr_score = 0;
	game->curr_diamonds = 0;
	game->curr_lives = 3;
	game->level_start_time = al_get_time();
	game->cam = create_camera(0, 0, width, height, 0, game->status_bar_height, 0.4f);
}

static void init_allegro(int width, int height, game_state* game) {
	// TODO: These should be moved somewhere else.
	al_init();
	al_install_keyboard();
	al_init_image_addon();
	al_init_font_addon();
	al_init_primitives_addon();

	game->display = al_create_display(width, height);
	game->font = al_create_builtin_font();
	game->queue = al_create_event_queue();
	game->render_timer = al_create_timer(1.0 / 60.0);
	game->update_timer = al_create_timer(1.0 / 10.0);

	al_register_event_source(game->queue, al_get_keyboard_event_source());
	al_register_event_source(game->queue, al_get_display_event_source(game->display));
	al_register_event_source(game->queue, al_get_timer_event_source(game->update_timer));
	al_register_event_source(game->queue, al_get_timer_event_source(game->render_timer));
}

static void load_map(game_state *game) {
	char *path = "level1.map";
	FILE *map1 = fopen(path, "r");

	if (!map1) {
	fprintf(stderr, "ERROR: Could not open map file '%s'\n", path);
		exit(1);
	}

	game->clean_map = map_from_file(map1);

	copy_map(&game->curr_map, &game->clean_map); // sets curr_map

	fclose(map1);
}

static void load_textures(int atlas_width, int atlas_height, int texture_size, game_state *game) {
	game->atlas_height = atlas_height;
	game->atlas_width = atlas_width;
	game->texture_size = texture_size;

	game->texture_atlas = load_texture("./resources/atlas.png");

	game->textures[DIRT] = load_sub_texture(game, DIRT);
	game->textures[ROCK] = load_sub_texture(game, ROCK);
	game->textures[ROCKFORD] = load_sub_texture(game, ROCKFORD);
	game->textures[DIAMOND] = load_sub_texture(game, DIAMOND);
	game->textures[AIR] = load_sub_texture(game, AIR);
	game->textures[BRICK] = load_sub_texture(game, BRICK);
	game->textures[STEEL] = load_sub_texture(game, STEEL);
	game->textures[DUST] = load_sub_texture(game, DUST);
}

void destroy_game(game_state *game) {
	free(game->curr_map.board);
	al_destroy_font(game->font);
	al_destroy_display(game->display);
	al_destroy_timer(game->render_timer);
	al_destroy_timer(game->update_timer);
	al_destroy_event_queue(game->queue);
}

static void kill_rockford(game_state *game) {
	explode_at(game->curr_map.rockford_x, game->curr_map.rockford_y, game->curr_map);

	reset_map(game);

	if (!--game->curr_lives) // if the lives get to zero it's game over.
		game_over(game);
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

	// If over unstable block, go to the right or left.
	if (get_block_property(x, y, IT_COLLIDES, game->curr_map) &&
		get_block_property(x, y + 1, IS_UNSTABLE, game->curr_map)) {

		if (!get_block_property(x - 1, y, IT_COLLIDES, game->curr_map) &&
			!get_block_property(x - 1, y + 1, IT_COLLIDES, game->curr_map)) {

			// Can move to the left
			set_block_at(x - 1, y, get_block_type(x, y, game->curr_map), game->curr_map);
			set_block_property(x - 1, y, HAS_CHANGED, game->curr_map);
			set_block_at(x, y, AIR, game->curr_map);

		} else if (!get_block_property(x + 1, y, IT_COLLIDES, game->curr_map) &&
					 !get_block_property(x + 1, y + 1, IT_COLLIDES, game->curr_map)) {

			// Can move to the right
			set_block_at(x + 1, y, get_block_type(x, y, game->curr_map), game->curr_map);
			set_block_property(x + 1, y, HAS_CHANGED, game->curr_map);
			set_block_at(x, y, AIR, game->curr_map);
		}

		return;
	}

	// Don't move otherwise
	unset_block_property(x, y, IN_MOVEMENT, game->curr_map);
}

void update_physics(game_state *game) {
	// We make the restriction that every map has a border of steel,
	// which don't need to be updated.
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
		game->should_quit = 1;

	int h_movement = 0;
	int v_movement = 0;

	h_movement =
		((c->key[ALLEGRO_KEY_D]) ? 1 : 0) - ((c->key[ALLEGRO_KEY_A]) ? 1 : 0);
	if (h_movement == 0)
		v_movement =
		((c->key[ALLEGRO_KEY_S]) ? 1 : 0) - ((c->key[ALLEGRO_KEY_W]) ? 1 : 0);

	move_rockford(h_movement, v_movement, game);
}

static void move_rockford(int x_amount, int y_amount, game_state *game) {
  int destx = game->curr_map.rockford_x + x_amount;
  int desty = game->curr_map.rockford_y + y_amount;
  
  if (get_block_property(destx, desty, IS_DIGGABLE, game->curr_map) ||
      !get_block_property(destx, desty, IT_COLLIDES, game->curr_map)) {
	// We can dig the block!
	if (get_block_type(destx, desty, game->curr_map) == DIAMOND)
		game->curr_diamonds++;

	set_block_at(game->curr_map.rockford_x, game->curr_map.rockford_y, AIR, game->curr_map);
    set_block_at(game->curr_map.rockford_x + x_amount, game->curr_map.rockford_y + y_amount, ROCKFORD, game->curr_map);

    game->curr_map.rockford_x += x_amount;
    game->curr_map.rockford_y += y_amount;

  } else if (get_block_property(destx, desty, IS_PUSHABLE, game->curr_map)) {
    // TODO : if next block not collides push the block, maybe add random so it seems difficult to push.
  }
}

static ALLEGRO_BITMAP *load_texture(char *path)
{
	ALLEGRO_BITMAP *texture = al_load_bitmap(path);
	if (!texture)
	{
		fprintf(stderr, "ERROR: Could not load texture '%s'", path);
		exit(1);
	}

	fprintf(stderr, "Loaded texture %s\n", path);

	return texture;
}

static ALLEGRO_BITMAP *load_sub_texture(game_state *game, block_type b)
{
	ALLEGRO_BITMAP *texture;
	int x = game->texture_size * (b % game->atlas_width);
	int y = game->texture_size * (b / game->atlas_width);

	texture = al_create_sub_bitmap(game->texture_atlas, x, y, game->texture_size,
			game->texture_size);

	if (!texture)
	{
		fprintf(stderr, "ERROR: Could not load sub texture '%s'", block_name(b));
		exit(1);
	}

	fprintf(stderr, "Loaded sub texture %s\n", block_name(b));

	return texture;
}

void render_status_bar(game_state *game) {
	char status_str[256];

	al_draw_filled_rectangle(0.0f, 0.0f, game->cam.width, game->status_bar_height, al_map_rgb(100, 100, 100)); 

	sprintf(status_str, "DIAMONDS: %d", game->curr_diamonds);
	al_draw_text(game->font, al_map_rgb(230, 230, 230), game->cam.width * 0.1f, game->status_bar_height * 1.0f / 3.0f, ALLEGRO_ALIGN_LEFT, status_str);

	sprintf(status_str, "SCORE: %d", game->curr_score);
	al_draw_text(game->font, al_map_rgb(230, 230, 230), game->cam.width * 0.5f, game->status_bar_height * 1.0f / 3.0f, ALLEGRO_ALIGN_CENTER, status_str);

	sprintf(status_str, "LIVES: %d", game->curr_lives);
	al_draw_text(game->font, al_map_rgb(235, 235, 235), game->cam.width * 0.9f, game->status_bar_height * 1.0f / 3.0f, ALLEGRO_ALIGN_RIGHT, status_str);
}

static void game_over(game_state* game) {
	fprintf(stderr, "DEBUG: Game over....\n");
}


static void reset_map(game_state *game) {
	block *tmp = game->curr_map.board;

	memcpy(&game->curr_map, &game->clean_map, sizeof(map));

	game->curr_map.board = tmp;

	memcpy(game->curr_map.board, game->clean_map.board, sizeof(block) * game->curr_map.width * game->curr_map.height);
}
