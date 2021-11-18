#include "camera.h"

#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <allegro5/allegro5.h>

#include "map.h"

camera create_camera(int x, int y, int width, int height, int offset_x, int offset_y, float zoom) {
	camera cam;
	cam.pos_x = x;
	cam.pos_y = y;
	cam.width = width;
	cam.height = height;
	cam.offset_x = offset_x;
	cam.offset_y = offset_y;
	cam.zoom = zoom;
	return cam;
}

float lerp(float start, float end, float t) {
	return start * (1 - t) + end * t;
}

void update_camera(int x, int y, float zoom, camera *cam) {
	float speed = 0.1f;
	cam->pos_x = (lerp(cam->pos_x, x, speed));
	cam->pos_y = (lerp(cam->pos_y, y, speed));
}

void render_camera(ALLEGRO_BITMAP *textures[], float texture_size, map m, camera cam) {
	al_clear_to_color(al_map_rgb(0, 0, 0));
  
	// Improves performance by sending the atlas texture only once per frame.
	al_hold_bitmap_drawing(true);

	ALLEGRO_TRANSFORM original, current;

	// Backup the original transform
	al_copy_transform(&original, al_get_current_transform());

	al_identity_transform(&current);
	
	// Align the drawing to the center
	al_translate_transform(&current, -texture_size / 2.0f, -texture_size / 2.0f);

	// Center the origin to the camera position.
	al_translate_transform(&current, -cam.pos_x * texture_size, -cam.pos_y * texture_size);

	// Scale everything
	al_scale_transform(&current, cam.zoom, cam.zoom);

	// Center origin to the middle of the screen.
	al_translate_transform(&current, cam.width / 2.0f, cam.height / 2.0f);

	al_use_transform(&current);

	for (int x = 0; x < m.width; x++) {
		for (int y = 0; y < m.height; y++) {
			block_type type = get_block_type(x, y, m);
			ALLEGRO_BITMAP* bitmap = textures[type];
			al_draw_bitmap(bitmap, x * texture_size, y * texture_size, 0);
		}
	}

	// Actually draw to the screen.
	al_hold_bitmap_drawing(false);

	// Go back to the old tranform
	al_use_transform(&original);
}
