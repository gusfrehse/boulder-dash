#include "input.h"
#include <allegro5/keycodes.h> 

void reset_input(input_controller *c) {
	memset(c->key, 0, sizeof(c->key));
}

void process_input(input_controller *c, ALLEGRO_EVENT e) {
	switch (e.type) {
	case ALLEGRO_EVENT_KEY_DOWN:
		c->key[e.keyboard.keycode] = KEY_REPEATING | KEY_TOUCHED;
		c->keys_touched++;
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
			c->keys_touched = 0;
		}
}
