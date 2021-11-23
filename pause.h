#ifndef GAME_PAUSE_H 
#define GAME_PAUSE_H 

#include "input.h"
#include "game_state.h"


void update_pause(input_controller* c, game_state *game);
void render_pause(game_state *game);

#endif
