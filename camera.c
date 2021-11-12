#include "camera.h"

#include <allegro5/allegro5.h>
#include "map.h"

static int clamp(int val, int min, int max) {
  if (val < min)
    return min;

  if (val > max)
    return max;

  return val;
}

camera create_camera(int x, int y, int width, int height, float zoom, int tile_size) {
	camera cam;
	cam.pos_x = x;
	cam.pos_y = y;
	cam.width = width;
	cam.height = height;
	cam.zoom = zoom;
	cam.tile_size = tile_size;
	return cam;
}

void update_camera(int x, int y, camera *cam) {
	cam->pos_x = x;
	cam->pos_y = y;
}

void render_camera(ALLEGRO_BITMAP *textures[], map m, camera cam) {
  al_clear_to_color(al_map_rgb(0, 0, 0));
  // Improves performance by sending the atlas texture only once per frame.
  al_hold_bitmap_drawing(true);

	int map_x, map_y;

	// in pixels
	int center_x = (cam.width / 2) - (cam.tile_size / 2);
	int center_y = (cam.height / 2) - (cam.tile_size / 2);

	for (int dy = -3; dy <= 3; dy++) {
		for (int dx = -3; dx <= 3; dx++) {
			map_x = clamp(cam.pos_x + dx, 0, m.width - 1);
			map_y = clamp(cam.pos_y + dy, 0, m.height - 1);

			block_type type = get_block_type(map_x, map_y, m);
			ALLEGRO_BITMAP *bitmap = textures[type];

			al_draw_scaled_bitmap(bitmap,
					0, 0,														// source x/y
					cam.tile_size, cam.tile_size,		// source w/h
					center_x + dx * cam.tile_size,	// destination x
					center_y + dy * cam.tile_size,	// destination y
					cam.tile_size, cam.tile_size,		// destination w/h
					0);
		}
	}

  // Actually draw to the screen.
  al_hold_bitmap_drawing(false);

  al_flip_display();
}

