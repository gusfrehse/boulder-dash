#include "pause.h"

void update_pause(input_controller* c, game_state *game) {
	if (c->key[ALLEGRO_KEY_ESCAPE])
		game->status = IN_GAME;

	if (c->key[ALLEGRO_KEY_ENTER]) {
		if (game->pause_selection == 0) {
			game->status = IN_GAME;
		} else if (game->pause_selection == 1) {
			game->status = SHOULD_QUIT;
		}
	}

}

void render_pause(game_state *game) {
	al_draw_filled_rectangle();

}
