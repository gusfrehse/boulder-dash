#ifndef INPUT_H
#define INPUT_H

#include <allegro5/allegro5.h>

// Similar to the Allegro Vivace tutorial.

// handles the case where the key is pressed in one frame and released in another
#define KEY_REPEATING 1

// Handles the case where the key pressed and released in between frames.
#define KEY_TOUCHED 2

typedef struct {
	unsigned char key[ALLEGRO_KEY_MAX];
	unsigned int keys_touched; 
} input_controller;

void reset_input(input_controller *c);
void process_input(input_controller *c, ALLEGRO_EVENT e);
void update_input(input_controller *c);

#endif
