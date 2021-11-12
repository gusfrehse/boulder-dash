#ifndef GAME_STOPWATCH_H
#define GAME_STOPWATCH_H

#include <time.h>

struct timespec timer_start();
long timer_end(struct timespec start_time);

#endif
