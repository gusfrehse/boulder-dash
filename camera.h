#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

#include <allegro5/allegro5.h>

#include "map.h"
#include "texture.h"

typedef struct {
	/// Represent where the camera is on the map in blocks.
	float pos_x, pos_y;

	/// Size of the viewport in pixels.
	int width, height;
	
	/// Offset of the viewport in relation to the window. In pixels.
	int offset_x, offset_y;

	/// Magnification, increase it to increase the size of the blocks.
	float zoom;
} camera;


camera create_camera(int x, int y, int width, int height, int offset_x, int offset_y, float zoom);
void update_camera(int x, int y, float zoom, camera *camera);
void render_camera(texture_system *ts, map m, camera cam);

#endif

