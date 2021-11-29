#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/bitmap.h>
#include <allegro5/events.h>

#include "input.h"
#include "map.h"
#include "camera.h"
#include "score.h"
#include "texture.h"
#include "sample.h"

enum status { IN_GAME, DEATH_ANIMATION, GAME_OVER, GAME_END, HELP, SHOULD_QUIT };

typedef struct game_state {
	ALLEGRO_DISPLAY *display;
	ALLEGRO_FONT *font;
	ALLEGRO_EVENT_QUEUE *queue;
	ALLEGRO_TIMER *realtime_timer;
	ALLEGRO_TIMER *oldschool_timer;

	sample_system sample_system;
	texture_system texture_system;

	char *score_path;
	
	enum status status;

	int selection; // For gui's (game end, game over)

	camera cam;
	map curr_map;
	map clean_map;

	int status_bar_height;

	score curr_score;
	score *previous_scores;
	
	int num_scores;

	int score_per_diamond;
	int score_per_second;
	int curr_diamonds;
	int needed_diamonds;
	int curr_lives;
	double max_time;
	double level_start_time;
	double death_animation_start_time;
} game_state;

void init_game(game_state *game, int width, int height, float zoom,
		float cam_speed, int concurrent_samples, float volume,
		char *atlas_path, char *level_path, char *score_path);
void destroy_game(game_state *game);

void start_level(game_state *game);
void revive_rockford(game_state *game);
void update_game(input_controller *c, game_state *game);
void update_physics(game_state *game);
void update_death_animation (game_state *game);
void render_status_bar(game_state *game);

#endif
