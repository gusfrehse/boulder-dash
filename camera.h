#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

#include <allegro5/allegro5.h>

#include "map.h"

typedef struct {
	/// Represent where it is in the map.
	int pos_x, pos_y;

	/// Size of the viewport in pixels.
	int width, height;

	/// Magnification, increase it to increase the size of the blocks.
	float zoom;

	int texture_size;
} camera;


camera create_camera(int x, int y, int width, int height, float zoom, int texture_size);
void update_camera(int x, int y, camera *camera);
void render_camera(ALLEGRO_BITMAP *textures[], map m, camera camera);

#endif

