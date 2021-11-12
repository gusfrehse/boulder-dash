#include "input.h"
#include <allegro5/keycodes.h> 

// Similar to the Allegro Vivace tutorial.

// handles the case where the key is pressed in one frame and released in another
#define KEY_REPEATING 1

// Handles the case where the key pressed and released in between frames.
#define KEY_TOUCHED 2

void reset_input(input_controller *c) {
	memset(c->key, 0, sizeof(c->key));
}
void process_input(input_controller *c, ALLEGRO_EVENT e) {
  switch (e.type) {
  case ALLEGRO_EVENT_KEY_DOWN:
		c->key[e.keyboard.keycode] = KEY_REPEATING | KEY_TOUCHED;
    break;

  case ALLEGRO_EVENT_KEY_UP:
		c->key[e.keyboard.keycode] &= KEY_TOUCHED; 
		break;

  default:
    break;
  }
}

void update_input(input_controller *c) {
		for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
			c->key[i] &= KEY_REPEATING;
		}
}
