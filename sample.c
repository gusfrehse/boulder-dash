#include "sample.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>

#include <stdio.h>

static ALLEGRO_SAMPLE* load_sample(char *path) {
	ALLEGRO_SAMPLE *sample = al_load_sample(path);
	if (!sample) {
		fprintf(stderr, "ERROR: Could not load sample '%s'.\n", path);
		exit(1);
	}

	fprintf(stderr, "Loaded sample '%s'.\n", path);

	return sample;
}

void load_sample_system(sample_system *ss, int concurrent_samples, float volume) {
	ss->volume = volume;

	if (!al_reserve_samples(concurrent_samples)) {
		fprintf(stderr, "ERROR: Could not reserve samples\n");
		exit(1);
	}

	ss->samples[FALLING] = load_sample("./resources/falling.wav");
	ss->samples[DIAMOND_PICKUP] = load_sample("./resources/diamond_pickup.wav");
	ss->samples[DIGGING] = load_sample("./resources/digging.wav");
	ss->samples[DEATH] = load_sample("./resources/death.wav");
	ss->samples[SELECTION] = load_sample("./resources/selection.wav");
}

void play_sample(sample s, sample_system *ss) {
	al_play_sample(ss->samples[s], ss->volume, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
}

void destroy_sample_system(sample_system *ss) {
	for (int i = 0; i < NUM_SAMPLES; i++) {
		al_destroy_sample(ss->samples[i]);
	}
}
