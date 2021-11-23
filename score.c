#include "score.h"

#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 512
#define NUM_SCORES 5

score* highest_scores(char *path, int n) {
	FILE *f = fopen(path, "r");
	if (!f) {
		fprintf(stderr, "ERROR: Could not open scores file '%s' for reading.\n", path);
		exit(1);
	}

	score* scores = calloc(n, sizeof(score));

	int i = 0;
	while (i < n &&
			fscanf(f, "%d %256[^\n]", &scores[i].score, scores[i].name) != EOF) {
		scores[i].name[255] = '\0';
		i++;
	}

	fclose(f);

	return scores;
}

void insert_score(char *path, score hs) {
	FILE *f = fopen(path, "r+");

	if (!f) {
		fprintf(stderr, "ERROR: Could not open scores file '%s' for reading and writing.\n", path);
		exit(1);
	}

	int test_score;

	while (fscanf(f, "%d %*[^\n] ", &test_score) > 0) {
		if (test_score < hs.score)
			break;
	}

	// End position
	fseek(f, 0L, SEEK_END);
	int end_pos = ftell(f);

	// Position that we will write our score at
	int curr_pos = ftell(f);
	
	// The amount of bytes we will have to push back
	int amount_to_move = end_pos - curr_pos;

	char *buff = calloc(amount_to_move, 1);
	if (fread(buff, 1, amount_to_move, f) != amount_to_move) {
		fprintf(stderr, "ERROR: Could not read score file '%s'\n", path);
		exit(1);
	}

	fseek(f, 0L, curr_pos);

	fprintf(f, "%d %s\n", hs.score, hs.name);
	fwrite(buff, 1, amount_to_move, f);

	fclose(f);
	return;
}

