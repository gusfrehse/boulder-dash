#ifndef GAME_EASTER_EGG
#define GAME_EASTER_EGG

#include "input.h"

/**
 * Easter egg state machine states.
 */
typedef enum easter_egg_state { EE_STEP_0, EE_STEP_1, EE_STEP_2, EE_STEP_3 } ee_state;

/**
 * Initializes an easter egg state machine.
 */
void init_ee_state(ee_state *s);

/**
 * Updates the easter egg state machine, based on the key input.
 */
void update_ee_state(ee_state *s, input_controller *c);

/**
 * Return if easter egg has been activated.
 */
int is_ee(ee_state s);

#endif
