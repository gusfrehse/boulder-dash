#ifndef GAME_SAMPLE_SYSTEM
#define GAME_SAMPLE_SYSTEM

#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>

/**
 * Possible sounds.
 */
typedef enum { FALLING, DIAMOND_PICKUP, DIGGING, DEATH, SELECTION, NUM_SAMPLES} sample;

/**
 * Sound system structure. Keeps track of the samples, volume and how many concurrent samples are possible.
 */
typedef struct {
	ALLEGRO_SAMPLE *samples[NUM_SAMPLES];
	int concurrent_samples;
	float volume;
} sample_system;

/**
 * Loads the samples from a predefined path.
 */
void load_sample_system(sample_system *ss, int concurrent_samples, float volume);

/**
 * Plays a sample from the sample_system.
 */
void play_sample(sample s, sample_system *ss);


/**
 * Deallocates the sample_system
 */
void destroy_sample_system(sample_system *ss);

#endif

