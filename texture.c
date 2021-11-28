#include "texture.h"

#include <allegro5/keycodes.h>
#include <stdio.h>
#include <math.h>

#include <allegro5/allegro5.h>

#include "map.h"
#include "input.h"
#include "easter_egg.h"

static ALLEGRO_BITMAP *load_texture(char *path) {
	ALLEGRO_BITMAP *texture = al_load_bitmap(path);
	if (!texture) {
		fprintf(stderr, "ERROR: Could not load texture '%s'.", path);
		exit(1);
	}

	fprintf(stderr, "Loaded texture %s.\n", path);

	return texture;
}

static ALLEGRO_BITMAP *load_animation_frame(texture t, int animation_frame, ALLEGRO_BITMAP* atlas, int t_size) {
	ALLEGRO_BITMAP *texture;
	int x = t_size * t;
	int y = t_size * animation_frame;

	texture = al_create_sub_bitmap(atlas, x, y, t_size, t_size);

	if (!texture) {
		fprintf(stderr, "ERROR: Could not load animation frame %d of texture '%d'", animation_frame, t);
		exit(1);
	}

	fprintf(stderr, "Loaded animation frame %d of texture %d\n", animation_frame, t);

	return texture;
}

void load_texture_system(texture_system *ts, char *atlas_path) {
	ts->atlas = load_texture(atlas_path);
	
	ts->texture_size = al_get_bitmap_width(ts->atlas) / T_NUM;
	ts->animation_frames = al_get_bitmap_height(ts->atlas) / ts->texture_size;

	ts->textures = calloc(T_NUM * ts->animation_frames, sizeof(ALLEGRO_BITMAP *));
	
	if (!ts->textures) {
		fprintf(stderr, "ERROR: Couldn't allocate memory for textures.");
		exit(1);
	}

	ts->curr_textures[DIRT] = T_DIRT;
	ts->curr_textures[ROCK] =  T_ROCK;
	ts->curr_textures[ROCKFORD] = T_ROCKFORD;
	ts->curr_textures[DIAMOND] = T_DIAMOND;
	ts->curr_textures[AIR] = T_AIR;
	ts->curr_textures[BRICK] = T_BRICK;
	ts->curr_textures[STEEL] = T_STEEL;
	ts->curr_textures[DUST] = T_DUST;
	ts->curr_textures[CLOSED_EXIT] = T_CLOSED_EXIT;
	ts->curr_textures[OPEN_EXIT] = T_OPEN_EXIT;

	for (int i = 0; i < T_NUM; i++) {
		for (int j = 0; j < ts->animation_frames; j++) {
			ts->textures[i + j * T_NUM] = load_animation_frame(i, j, ts->atlas, ts->texture_size);
		}
	}
}

void update_texture_system(input_controller *c, texture_system *ts, ee_state ee) {

	// Update rockford texture so it reflect the input.
	if (c->key[ALLEGRO_KEY_D] || c->key[ALLEGRO_KEY_RIGHT]) {
		ts->curr_textures[ROCKFORD] = T_ROCKFORD_R;
	} else if (c->key[ALLEGRO_KEY_A] || c->key[ALLEGRO_KEY_LEFT]) {
		ts->curr_textures[ROCKFORD] = T_ROCKFORD_L;
	} else {
		ts->curr_textures[ROCKFORD] = T_ROCKFORD;
	}

	if (is_ee(ee)) {
		ts->curr_textures[ROCK] = T_EGG;
	}

}

void draw_texture_animated(int x, int y, texture t, texture_system *ts) {
	double time = al_get_time();

	int frame = ts->animation_frames * fmod(time, 1.0);

	assert(frame < ts->animation_frames);

	al_draw_bitmap(ts->textures[frame * T_NUM + t], x, y, 0);
}

void destroy_texture_system(texture_system *ts) {
	for (int i = 0; i < T_NUM * ts->animation_frames; i++) {
		al_destroy_bitmap(ts->textures[i]);
	}

	free(ts->textures);

	al_destroy_bitmap(ts->atlas);
}
