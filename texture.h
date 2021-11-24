#ifndef GAME_TEXTURE_H
#define GAME_TEXTURE_H

#include <allegro5/allegro5.h>

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
	T_NUM
} texture;

typedef struct {
	ALLEGRO_BITMAP **textures;
	ALLEGRO_BITMAP *atlas;
	int animation_frames;
	int texture_size;
} texture_system;

void load_texture_system(texture_system *ts, char *atlas_path);
void draw_texture_animated(int x, int y, texture t, texture_system *ts);

#endif
