// GRR20203927 Gustavo Silveira Frehse
#ifndef GAME_HELP_H
#define GAME_HELP_H

#include "game_state.h"

/**
 * Update function for the help.
 */
void update_help(input_controller *c, game_state* game);

/**
 * Render function for the help.
 */
void render_help(game_state* game);

#endif

