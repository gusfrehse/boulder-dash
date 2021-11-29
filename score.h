#ifndef GAME_CORE_H
#define GAME_CORE_H

#include <stdio.h>

typedef struct {
	int score;
	char name[256];
} score;

/**
 * Loads the n first scores from a score file in the specified path.
 * A score file is an ascii file with each line containing a score (int) a space (' ') and a name with less than 256 characters.
 * The score file is assumed to be ordered from highest to lowest.
 */
score* load_scores(char *path, int n);


/**
 * Inserts a score to a file containing highscores.
 * Keeps the score file ordered.
 */
void insert_score(char *path, score hs);


#endif
