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
#include "help.h"
#include "camera.h"

#define WIDTH 1024
#define HEIGHT 576
#define ZOOM (1.0f)

#define ATLAS_TEXTURE_W 3 // Number of sprites in a line of the atlas
#define ATLAS_TEXTURE_H 3 // Number of sprites in a column of the atlas
#define TEXTURE_SIZE 100 // Number of pixels in one side of a texture

#define SCORE_FILE_PATH "./resources/score"
#define LEVEL_FILE_PATH "./resources/level.map"
#define ATLAS_FILE_PATH "./resources/atlas.png"

int main(void) {
	ALLEGRO_EVENT event;

	game_state game;
	init_game(&game, WIDTH, HEIGHT, ZOOM, ATLAS_FILE_PATH, LEVEL_FILE_PATH, SCORE_FILE_PATH);

	input_controller controller;
	reset_input(&controller);

	al_start_timer(game.realtime_timer);
	al_start_timer(game.oldschool_timer);

	while (game.status != SHOULD_QUIT) {
		al_wait_for_event(game.queue, &event);
		switch (event.type) {
		case ALLEGRO_EVENT_KEY_DOWN:
		case ALLEGRO_EVENT_KEY_UP:
			process_input(&controller, event);
			break;
		case ALLEGRO_EVENT_TIMER:
			if (event.timer.source == game.realtime_timer) {
				if (game.status == IN_GAME) {
					update_camera(game.curr_map.rockford_x, game.curr_map.rockford_y, game.cam.zoom, &game.cam);
					render_camera(&game.texture_system, game.curr_map, game.cam);
					render_status_bar(&game);
				} else if (game.status == GAME_OVER) {
					update_game_over(&controller, &game);
					render_game_over(&game);
				} else if (game.status == HELP) {
					update_help(&controller, &game);
					render_help(&game);
				}
				al_flip_display();
			} else if (event.timer.source == game.oldschool_timer) {
				update_physics(&game);
				update_input(&controller);
				update_game(&controller, &game);
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
