#ifndef INPUT_H
#define INPUT_H

#include <allegro5/allegro5.h>

// Similar to the Allegro Vivace tutorial.

// Handles the case where the key is pressed in one frame and released in another
#define KEY_REPEATING 1

// Handles the case where the key pressed and released in between frames.
#define KEY_TOUCHED 2

typedef struct {
	/// Keeps track of the state of every key.
	unsigned char key[ALLEGRO_KEY_MAX];

	/// Keeps track of how many keys are at least touched this frame.
	unsigned int keys_touched; 
} input_controller;

/**
 * Zero out the input controller
 */
void reset_input(input_controller *c);

/**
 * Called every key event, updates the controller based on the event.
 */
void process_input(input_controller *c, ALLEGRO_EVENT e);

/**
 * Update the controller so only the repeating keys are left
 */
void update_input(input_controller *c);

#endif
