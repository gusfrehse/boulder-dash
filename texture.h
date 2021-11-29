// GRR20203927 Gustavo Silveira Frehse
#ifndef GAME_TEXTURE_H
#define GAME_TEXTURE_H

#include <allegro5/allegro5.h>

#include "map.h"
#include "input.h"
#include "easter_egg.h"

/**
 * Enum with possible textures.
 */
typedef enum {
	T_DIRT,
	T_ROCK,
	T_ROCKFORD_L,
	T_ROCKFORD_R,
	T_ROCKFORD,
	T_DIAMOND,
	T_AIR,
	T_BRICK,
	T_STEEL,
	T_DUST,
	T_CLOSED_EXIT,
	T_OPEN_EXIT,
	T_EGG,
	T_NUM
} texture;

/**
 * Texture system struture.
 * Keeps track of the textures and it's animations, as well as the current texture assigned to each block type.
 */
typedef struct {
	ALLEGRO_BITMAP **textures;
	ALLEGRO_BITMAP *atlas;
	int animation_frames;
	int texture_size;
	texture curr_textures[NUM_BLOCKS];
} texture_system;

/**
 * Loads the texture system from an atlas.
 */
void load_texture_system(texture_system *ts, char *atlas_path);

/**
 * Updates the texture system based on input. Mostly to change a texture assigned to a block type.
 */
void update_texture_system(input_controller *c, texture_system *ts, ee_state ee);

/**
 * Draw function.
 */
void draw_texture_animated(int x, int y, texture t, texture_system *ts);

/**
 * Deallocate resources used by the texture system.
 */
void destroy_texture_system(texture_system *ts);

#endif
