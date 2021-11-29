#ifndef GAME_PAUSE_H 
#define GAME_PAUSE_H 

#include "input.h"
#include "game_state.h"


/**
 * Update function for the pause part.
 */
void update_pause(input_controller* c, game_state *game);

/**
 * Render function for the pause part.
 */
void render_pause(game_state *game);

#endif
