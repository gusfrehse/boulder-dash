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

camera create_camera(int x, int y, int width, int height, float zoom, int texture_size) {
	camera cam;
	cam.pos_x = x;
	cam.pos_y = y;
	cam.width = width;
	cam.height = height;
	cam.zoom = zoom;
	cam.texture_size = texture_size;
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
	
	int draw_size = (int) (cam.zoom * cam.texture_size);
	
	// in pixels
	int center_x = (cam.width / 2) - (draw_size / 2);
	int center_y = (cam.height / 2) - (draw_size / 2);
	
	int number_blocks_h = 1 + center_x / draw_size;
	int number_blocks_v = 1 + center_y / draw_size;

	for (int dy = -number_blocks_v; dy <= number_blocks_v; dy++) {
		for (int dx = -number_blocks_h; dx <= number_blocks_h; dx++) {
			map_x = clamp(cam.pos_x + dx, 0, m.width - 1);
			map_y = clamp(cam.pos_y + dy, 0, m.height - 1);

			block_type type = get_block_type(map_x, map_y, m);
			ALLEGRO_BITMAP *bitmap = textures[type];

			al_draw_scaled_bitmap(bitmap,
					0, 0,								// source x/y
					cam.texture_size, cam.texture_size,	// source w/h
					center_x + dx * draw_size,			// destination x
					center_y + dy * draw_size,			// destination y
					draw_size, draw_size,				// destination w/h
					0);
		}
	}

  // Actually draw to the screen.
  al_hold_bitmap_drawing(false);

  al_flip_display();
}