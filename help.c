#include "help.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

#include "game_state.h"
#include "input.h"

static const char *help_text =
	"HELP\n"
	"Use W/A/S/D or the arrow keys to move.\n"
	"Collect the diamonds to increase your score.\n"
	"Be careful with the rocks! If one falls over you, you die!\n"
	"You have 3 lives at the start.\n" 
	"Press -/= to decrease/increase the zoom.\n" 
	"Press Esc at any time to exit.\n"
	"Press Enter to continue.\n";

void update_help(input_controller *c, game_state *game) {
	if (c->key[ALLEGRO_KEY_ENTER]) {
		game->status = IN_GAME;
	}
}

void render_help(game_state *game) {
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_multiline_text(game->font, al_map_rgb(255, 255, 255),
						   game->cam.width * 0.5f, game->cam.height * 0.33f,
						   game->cam.width * 0.8f, 35.0f, ALLEGRO_ALIGN_CENTER,
						   help_text);
}
