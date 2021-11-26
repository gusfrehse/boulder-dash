#ifndef GAME_SAMPLE_SYSTEM
#define GAME_SAMPLE_SYSTEM

#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>

typedef enum { FALLING, DIAMOND_PICKUP, DIGGING, DEATH, SELECTION, NUM_SAMPLES} sample;

typedef struct {
	ALLEGRO_SAMPLE *samples[NUM_SAMPLES];
	int concurrent_samples;
	float volume;
} sample_system;

void load_sample_system(sample_system *ss, int concurrent_samples, float volume);
void play_sample(sample s, sample_system *ss);
void destroy_sample_system(sample_system *ss);

#endif

