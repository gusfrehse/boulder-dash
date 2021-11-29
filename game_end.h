// GRR20203927 Gustavo Silveira Frehse
#ifndef GAME_GAME_END_H
#define GAME_GAME_END_H

#include "game_state.h"

/**
 * Update function for the game end part.
 */
void update_game_end(input_controller *c, game_state* game);


/**
 * Render function for the game end part.
 */
void render_game_end(game_state* game);


#endif

