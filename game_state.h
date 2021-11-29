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

/**
 * Possible game states.
 */
enum status { IN_GAME, DEATH_ANIMATION, GAME_OVER, GAME_END, HELP, SHOULD_QUIT };

/**
 * Main game structure, keeps most of the game data
 */
typedef struct game_state {
	// Allegro things
	ALLEGRO_DISPLAY *display;
	ALLEGRO_FONT *font;
	ALLEGRO_EVENT_QUEUE *queue;

	// Fast timer (60Hz).
	ALLEGRO_TIMER *realtime_timer;

	// Slow timer (10Hz).
	ALLEGRO_TIMER *oldschool_timer;

	sample_system sample_system;
	texture_system texture_system;

	// Score file path.
	char *score_path;
	
	// Current game state.
	enum status status;

	// For gui's (game end, game over).
	int selection; 

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
	int curr_lives;
	double max_time;
	double level_start_time;
	double death_animation_start_time;
} game_state;

/**
 * Init a game_state structure with the parameters.
 */
void init_game(game_state *game, int width, int height, float zoom,
		float cam_speed, int concurrent_samples, float volume,
		char *atlas_path, char *level_path, char *score_path);

/**
 * Releases allocated resources for a game_state.
 */
void destroy_game(game_state *game);

/**
 * Used at the start of a level to set things up (map, lives, score, etc.).
 */
void start_level(game_state *game);

/**
 * Used after Rockford dies. Resets some but not everything ie. time is not resetted.
 */
void revive_rockford(game_state *game);

/**
 * Main update function. Makes changes based on input (input_controller).
 */
void update_game(input_controller *c, game_state *game);

/**
 * Update the physics (ie. move objects with gravity).
 */
void update_physics(game_state *game);

/**
 * Update function for when the state is DEATH_ANIMATION. Goes back to IN_GAME automatically.
 */
void update_death_animation (game_state *game);

/**
 * Render the status bar on top, showing diamonds, score, time and lives.
 */
void render_status_bar(game_state *game);

#endif
