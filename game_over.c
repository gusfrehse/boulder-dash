#include "game_over.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

#include "input.h"
#include "game_state.h"
#include "score.h"


void update_game_over(input_controller *c, game_state* game) {
	if (game->selection != 0 && game->selection != 1) { // check if selection is valid
		game->selection = 0;
	}

	if (c->key[ALLEGRO_KEY_LEFT] || c->key[ALLEGRO_KEY_A]) {
		play_sample(SELECTION, &game->sample_system);
		game->selection = 0;
	}

	if (c->key[ALLEGRO_KEY_RIGHT] || c->key[ALLEGRO_KEY_D]) {
		play_sample(SELECTION, &game->sample_system);
		game->selection = 1;
	}

	if (c->key[ALLEGRO_KEY_ENTER]) {
		play_sample(SELECTION, &game->sample_system);

		if (game->selection == 1)
			game->status = SHOULD_QUIT;
		else
			game->status = IN_GAME;

		start_level(game);
	}
}

void render_game_over(game_state* game) {
	/*
	 * GAME OVER
	 * RETRY?
	 * Yes No
	 */
	ALLEGRO_COLOR selection_color = al_map_rgb(255, 255, 255);
	ALLEGRO_COLOR non_selection_color = al_map_rgb(120, 120, 120);

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_draw_text(game->font, al_map_rgb(255, 255, 255), game->cam.width * 0.5f, game->cam.height * 0.4f, ALLEGRO_ALIGN_CENTER, "GAME OVER");

	al_draw_text(game->font, al_map_rgb(255, 255, 255), game->cam.width * 0.5f, game->cam.height * 0.6f, ALLEGRO_ALIGN_CENTER, "RETRY?");
	al_draw_text(game->font, game->selection == 0 ? selection_color : non_selection_color, game->cam.width * 0.33f, game->cam.height * 0.8f, ALLEGRO_ALIGN_CENTER, "YES");
	al_draw_text(game->font, game->selection == 1 ? selection_color : non_selection_color, game->cam.width * 0.66f, game->cam.height * 0.8f, ALLEGRO_ALIGN_CENTER, "NO");
}


