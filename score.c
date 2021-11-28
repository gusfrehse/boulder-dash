#include "score.h"

#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 512
#define NUM_SCORES 5

score* load_scores(char *path, int n) {
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
	FILE *f = fopen(path, "r+b");

	if (!f) {
		fprintf(stderr, "ERROR: Could not open scores file '%s' for reading and writing.\n", path);
		exit(1);
	}

	int test_score;
	int bytes_scanned;
	int items_scanned;

	while ((items_scanned = fscanf(f, "%d %*[^\n] %n", &test_score, &bytes_scanned)) > 0) {
		fprintf(stderr, "DEBUG: Comparing %d < %d. Scanned: %d\n", test_score, hs.score, bytes_scanned);
		if (test_score < hs.score) {
			fprintf(stderr, "DEBUG: breaking out of loop\n");
			break;
		}
	}
	
	// fscanf buggy, %n is returning the last value even though the fscanf returns -1. 
	// So we need this if and the code is much worse :/ .
	if (items_scanned > 0) {
		// We are after the line we should be, so we go back.
		fseek(f, -bytes_scanned, SEEK_CUR);

		// Position that we will write our score at
		int curr_pos = ftell(f);
		
		// End position
		fseek(f, 0L, SEEK_END);
		int end_pos = ftell(f);

		// The amount of bytes we will have to push back
		int amount_to_move = end_pos - curr_pos;

		// Go back to original position to read the data
		fseek(f, curr_pos, SEEK_SET);

		char *buff = calloc(amount_to_move, 1);
		if (fread(buff, 1, amount_to_move, f) != amount_to_move) {
			fprintf(stderr, "ERROR: Could not read score file '%s'\n", path);
			exit(1);
		}

		// Go back to the original position to write the new data
		fseek(f, curr_pos, SEEK_SET);

		fprintf(f, "%d %s\n", hs.score, hs.name);

		// Write the old data afterwards
		fwrite(buff, 1, amount_to_move, f);
	} else {
		// We did not find any score smaller so we just append
		fprintf(f, "%d %s\n", hs.score, hs.name);
	}

	fclose(f);
	return;
}

