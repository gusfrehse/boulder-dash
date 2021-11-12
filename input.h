#ifndef INPUT_H
#define INPUT_H

#include <allegro5/allegro5.h>

typedef struct {
	unsigned char key[ALLEGRO_KEY_MAX];
} input_controller;

void reset_input(input_controller *c);
void process_input(input_controller *c, ALLEGRO_EVENT e);
void update_input(input_controller *c);

#endif
