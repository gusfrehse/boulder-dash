#include "easter_egg.h"

#include <allegro5/keycodes.h>

#include "input.h"


void init_ee_state(ee_state *s) {
	*s = EE_STEP_0;
}

void update_ee_state(ee_state *s, input_controller *c) {
	if (c->keys_touched > 0) {
		switch (*s) {
		case EE_STEP_0:
			if (c->key[ALLEGRO_KEY_E] & KEY_TOUCHED) {
				*s += 1;
				return;
			}
			break;
		case EE_STEP_1:
			if (c->key[ALLEGRO_KEY_G] & KEY_TOUCHED) {
				*s += 1;
				return;
			}
			break;
		case EE_STEP_2:
			if (c->key[ALLEGRO_KEY_G] & KEY_TOUCHED) {
				*s += 1;
				return;
			}
			break;
		case EE_STEP_3:
			return;
		}

		// A key was pressed that wasn't part of the easter egg so we reset the state.
		*s = EE_STEP_0;
	}
}

int is_ee(ee_state s) {
	return (s == EE_STEP_3);
}
