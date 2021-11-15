#include "game_state.h"

#include <allegro5/allegro_image.h>
#include <allegro5/altime.h>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/drawing.h>
#include <allegro5/timer.h>

#include <stdio.h>
#include <string.h>

#include "input.h"
#include "map.h"
#include "camera.h"

static ALLEGRO_BITMAP *load_texture(char *path);
static ALLEGRO_BITMAP *load_sub_texture(game_state *game, block_type b);

void init_game(game_state *game, int width, int height, float zoom,
               int atlas_width, int atlas_height, int texture_size,
			   char *level_path)
{
  // TODO: These should be moved somewhere else.
  al_init();
  al_install_keyboard();
  al_init_image_addon();
  al_init_font_addon();

  game->display = al_create_display(width, height);
  game->font = al_create_builtin_font();
  game->queue = al_create_event_queue();
  game->render_timer = al_create_timer(1.0 / 60.0);
  game->update_timer = al_create_timer(1.0 / 10.0);

  // TODO: These also should be moved to somewhere else.
  al_register_event_source(game->queue, al_get_keyboard_event_source());
  al_register_event_source(game->queue,
		  al_get_display_event_source(game->display));
  al_register_event_source(game->queue,
		  al_get_timer_event_source(game->update_timer));
  al_register_event_source(game->queue,
		  al_get_timer_event_source(game->render_timer));

  // Create board
  FILE *map1 = fopen("level1.map", "r");
  if (!map1) {
    fprintf(stderr, "ERROR: Could not open map\n");
		exit(1);
  }
  game->curr_map = map_from_file(map1);
  fclose(map1);

  // Load textures
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

	// Rendering thing
  game->cam = create_camera(0, 0, width, height, 0, 0, 0.4f);
}

void destroy_game(game_state *game) {
  free(game->curr_map.board);
  al_destroy_font(game->font);
  al_destroy_display(game->display);
  al_destroy_timer(game->render_timer);
  al_destroy_timer(game->update_timer);
  al_destroy_event_queue(game->queue);
}

static void apply_physics(int x, int y, game_state *game) {
  // If rock over rock -> rock over falls if possible
  // If rock over dirt -> rock stays
  // If rock over air -> rock moves down
  // If rock over rockford -> if rock in already in movement -> rockford dies ->
  // else -> rock stays

  // We only change blocks that have gravity.
  if (!get_block_property(x, y, HAS_GRAVITY, game->curr_map)) {
    return;
  }
  // write_block(stderr, x, y, game->curr_map);

  // If we already changed current this frame we don't change it again.
  if (get_block_property(x, y, HAS_CHANGED, game->curr_map)) {
    return;
  }

  // If below don't collides we can move current down
  if (!get_block_property(x, y + 1, IT_COLLIDES, game->curr_map)) {
    set_block_at(x, y + 1, get_block_type(x, y, game->curr_map),
                 game->curr_map);
    set_block_at(x, y, AIR, game->curr_map);
    set_block_property(x, y + 1, IN_MOVEMENT | HAS_CHANGED, game->curr_map);
    return;
  }

  // If over colliding block, go to the right or left.
  if (get_block_property(x, y, IT_COLLIDES, game->curr_map) &&
      get_block_property(x, y + 1, IS_UNSTABLE, game->curr_map)) {

    if (!get_block_property(x - 1, y, IT_COLLIDES, game->curr_map) &&
        !get_block_property(x - 1, y + 1, IT_COLLIDES, game->curr_map)) {
      // Can move to the left
      set_block_at(x - 1, y, get_block_type(x, y, game->curr_map),
                   game->curr_map);
      set_block_property(x - 1, y, HAS_CHANGED, game->curr_map);
      set_block_at(x, y, AIR, game->curr_map);
    } else if (!get_block_property(x + 1, y, IT_COLLIDES, game->curr_map) &&
               !get_block_property(x + 1, y + 1, IT_COLLIDES, game->curr_map)) {
      // Can move to the right
      set_block_at(x + 1, y, get_block_type(x, y, game->curr_map),
                   game->curr_map);
      set_block_property(x + 1, y, HAS_CHANGED, game->curr_map);
      set_block_at(x, y, AIR, game->curr_map);
    }
    return;
  }
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

  move_rockford(h_movement, v_movement, &game->curr_map);
  update_camera(game->curr_map.rockford_x, game->curr_map.rockford_y,
		  game->cam.zoom, &game->cam);
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
