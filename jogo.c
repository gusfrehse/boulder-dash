#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/color.h>
#include <allegro5/display.h>
#include <allegro5/events.h>
#include <allegro5/timer.h>
#include <stdio.h>

#include "game_state.h"
#include "input.h"
#include "game_over.h"
#include "game_end.h"
#include "help.h"
#include "camera.h"
#include "easter_egg.h"

#define WIDTH 1024
#define HEIGHT 576
#define ZOOM (3.0f)
#define CAM_SPEED (0.1f)

#define ATLAS_TEXTURE_W 3 // Number of sprites in a line of the atlas
#define ATLAS_TEXTURE_H 3 // Number of sprites in a column of the atlas
#define TEXTURE_SIZE 100 // Number of pixels in one side of a texture

#define CONCURRENT_SAMPLES 6
#define VOLUME (0.7f)

#define SCORE_FILE_PATH "./resources/score"
#define LEVEL_FILE_PATH "./resources/level.map"
#define ATLAS_FILE_PATH "./resources/atlas.png"

int main(void) {
	ALLEGRO_EVENT event;

	// Initializing the main game structure.
	game_state game;
	init_game(&game, WIDTH, HEIGHT, ZOOM, CAM_SPEED, CONCURRENT_SAMPLES, VOLUME, ATLAS_FILE_PATH, LEVEL_FILE_PATH, SCORE_FILE_PATH);

	// Setup the input controller.
	input_controller controller;
	reset_input(&controller);

	// easter egg
	ee_state ee;
	init_ee_state(&ee);

	al_start_timer(game.realtime_timer);   // this timer is 60Hz.
	al_start_timer(game.oldschool_timer);  // this one is 10Hz.

	while (game.status != SHOULD_QUIT) {
		al_wait_for_event(game.queue, &event);
		switch (event.type) {
		case ALLEGRO_EVENT_KEY_DOWN:
		case ALLEGRO_EVENT_KEY_UP:
			process_input(&controller, event);
			break;
		case ALLEGRO_EVENT_TIMER:
			if (event.timer.source == game.realtime_timer) {
				if (game.status == IN_GAME || game.status == DEATH_ANIMATION) {
					// Update camera movement.
					update_camera(game.curr_map.rockford_x, game.curr_map.rockford_y, game.cam.zoom, &game.cam);

					// Update the textures.
					update_texture_system(&controller, &game.texture_system, ee);

					// Rendering.
					render_camera(&game.texture_system, game.curr_map, game.cam);
					render_status_bar(&game);
				} else if (game.status == GAME_OVER) {

					update_game_over(&controller, &game);
					render_game_over(&game);
				} else if (game.status == HELP) {

					update_help(&controller, &game);
					render_help(&game);
				} else if (game.status == GAME_END) {
					update_game_end(&controller, &game);
					render_game_end(&game);
				}

				// Input updating.
				update_ee_state(&ee, &controller);
				update_input(&controller);

				// "Apply" the drawing to the screen.
				al_flip_display();
			} else if (event.timer.source == game.oldschool_timer) {
				// We only update here so we get the retro feeling easily.
				
				if (game.status == IN_GAME) {
					update_game(&controller, &game);
					update_physics(&game);
				} else if (game.status == DEATH_ANIMATION) {
					update_physics(&game);
					update_death_animation(&game);
				}

			}
			break;
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
				game.status = SHOULD_QUIT;
				break;
		}
	}

	destroy_game(&game);

	return 0;
}
