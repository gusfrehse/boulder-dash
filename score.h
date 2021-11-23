#ifndef GAME_CORE_H
#define GAME_CORE_H

#include <stdio.h>

typedef struct {
	int score;
	char name[256];
} score;

/**
 * Reads from a file containing highscores the highest score
 */
score* highest_scores(char *path, int n);


/**
 * Inserts a highscore to a file containing highscores.
 */
void insert_score(char *path, score hs);


#endif
