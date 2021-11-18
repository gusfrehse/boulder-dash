#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/color.h>
#include <allegro5/display.h>
#include <allegro5/events.h>
#include <allegro5/timer.h>
#include <stdio.h>

#include "game_state.h"
#include "input.h"
#include "stopwatch.h"
#include "camera.h"

#define WIDTH 1024
#define HEIGHT 576
#define ZOOM (1.0f)

#define ATLAS_TEXTURE_W 3 // Number of sprites in a line of the atlas
#define ATLAS_TEXTURE_H 3 // Number of sprites in a column of the atlas
#define TEXTURE_SIZE 100 // Number of pixels in one side of a texture

int main(void) {
	ALLEGRO_EVENT event;

	game_state game;
	init_game(&game, WIDTH, HEIGHT, ZOOM, ATLAS_TEXTURE_W, ATLAS_TEXTURE_H, TEXTURE_SIZE, "path/to/file");

	input_controller controller;
	reset_input(&controller);

	al_start_timer(game.render_timer);
	al_start_timer(game.update_timer);

	while (!game.should_quit) {
		al_wait_for_event(game.queue, &event);

		switch (event.type) {
		case ALLEGRO_EVENT_KEY_DOWN:
		case ALLEGRO_EVENT_KEY_UP:
			process_input(&controller, event);
			break;
		case ALLEGRO_EVENT_TIMER:
			if (event.timer.source == game.render_timer) {
				//struct timespec t = timer_start();
				update_camera(game.curr_map.rockford_x, game.curr_map.rockford_y, game.cam.zoom, &game.cam);
				render_camera(game.textures, TEXTURE_SIZE, game.curr_map, game.cam);
				render_status_bar(&game);
				al_flip_display();
				//long long time = timer_end(t);
				//fprintf(stderr, "DEBUG: Render time %f s\n", (double)time * 1e-9);
			} else if (event.timer.source == game.update_timer) {
				//struct timespec t = timer_start();
				update_physics(&game);
				update_input(&controller);
				update_game(&controller, &game);
				//long long time = timer_end(t);
				//fprintf(stderr, "DEBUG: Update time %f s\n", (double)time * 1e-9);
			}
			break;
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
				fprintf(stderr, "DEBUG: Closing time!\n");
				game.should_quit = 1;
				break;
			}
	}

	destroy_game(&game);

	return 0;
}
