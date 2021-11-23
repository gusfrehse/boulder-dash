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
		game->selection = 0;
	}

	if (c->key[ALLEGRO_KEY_RIGHT] || c->key[ALLEGRO_KEY_D]) {
		game->selection = 1;
	}

	if (c->key[ALLEGRO_KEY_ENTER]) {
		if (game->selection == 0)
			insert_score(game->score_path, game->curr_score);

		game->status = IN_GAME;
		start_level(game);
	}
}

void render_game_over(game_state* game) {
	/*
	 * GAME OVER
	 * PREVIOUS SCORES:
	 * ...
	 * ...
	 * ...
	 * ... 
	 * ...
	 * ...
	 * ...
	 * SAVE SCORE?
	 * YES NO
	 */
	ALLEGRO_COLOR selection_color = al_map_rgb(255, 255, 255);
	ALLEGRO_COLOR non_selection_color = al_map_rgb(120, 120, 120);

	int num_scores = 15;
	score *previous_scores = highest_scores(game->score_path, num_scores);

	char tmp_score[512];
	char curr_score[512];
	sprintf(curr_score, "%d", game->curr_score.score);

	al_clear_to_color(al_map_rgb(0, 0, 0));

	al_draw_text(game->font, al_map_rgb(255, 255, 255), game->cam.width * 0.5f, game->cam.height * 0.1f, ALLEGRO_ALIGN_CENTER, "GAME OVER");
	al_draw_text(game->font, al_map_rgb(255, 255, 255), game->cam.width * 0.5f, game->cam.height * 0.2f, ALLEGRO_ALIGN_CENTER, "PREVIOUS SCORES:");


	// We have 0.5f * height to put the scores
	for (int i = 0; i < num_scores; i++) {
		sprintf(tmp_score, "%d", previous_scores[i].score);
		al_draw_text(game->font, al_map_rgb(255, 255, 255), game->cam.width * 0.48f, game->cam.height * (0.3f + (0.5f / num_scores) * i), ALLEGRO_ALIGN_RIGHT, tmp_score);
		al_draw_text(game->font, al_map_rgb(255, 255, 255), game->cam.width * 0.52f, game->cam.height * (0.3f + (0.5f / num_scores) * i), ALLEGRO_ALIGN_LEFT, previous_scores[i].name);
	}

	al_draw_text(game->font, al_map_rgb(255, 255, 255), game->cam.width * 0.5f, game->cam.height * 0.8f, ALLEGRO_ALIGN_CENTER, "SAVE SCORE?");
	al_draw_text(game->font, game->selection == 0 ? selection_color : non_selection_color, game->cam.width * 0.33f, game->cam.height * 0.9f, ALLEGRO_ALIGN_CENTER, "YES");
	al_draw_text(game->font, game->selection == 1 ? selection_color : non_selection_color, game->cam.width * 0.66f, game->cam.height * 0.9f, ALLEGRO_ALIGN_CENTER, "NO");
}


