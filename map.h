// GRR20203927 Gustavo Silveira Frehse
#ifndef GAME_MAP_H
#define GAME_MAP_H

#include <stdio.h>

#define MAP_MAX_WIDTH 512  // in blocks, don't include newlines or null bytes
#define MAP_MAX_HEIGHT 512

/**
 * Enum of all blocktypes.
 */
typedef enum {
	DIRT,
	ROCK,
	ROCKFORD,
	DIAMOND,
	AIR,
	BRICK,
	STEEL,
	DUST,
	CLOSED_EXIT,
	OPEN_EXIT,
	NUM_BLOCKS
} block_type;

/**
 * Properties a block can have.
 */
typedef enum {
	HAS_GRAVITY =   0x1,    // Can be push down every update if nothing blocks it.
	IT_COLLIDES =   0x2,    // Can prevent a block with gravity to fall.
	IS_UNSTABLE =   0x4,    // In the case of a block with gravity is over it, if 
							// the above block slips to a side.
	IN_MOVEMENT =   0x8,    // If movement have started.
	IS_PUSHABLE =  0x10,    // If Rockford can push the block.
	IS_DIGGABLE =  0x20,    // If Rockford can dig/collect the block.
	HAS_CHANGED =  0x40,    // If the block changed this frame already.
	UNBREAKABLE =  0x80,    // If the block can be broke (exploded for example).
	DOES_VANISH = 0x100,    // If the block can be broke (exploded for example).
} block_property;

/**
 * A block.
 */
typedef struct {
	block_type type;
	unsigned int properties;
} block;


/**
 * Map.
 */
typedef struct {
	// Block matrix.
	block *board;
	int width, height;
	int rockford_x, rockford_y;
	int exit_x, exit_y;
	int needed_diamonds;
} map;

block create_block(block_type type);
char* block_name(block_type b);
void write_block(FILE *f, int x, int y, map m);

map map_from_file(FILE* f);
map map_from_string(char * map_str);

void open_exit(map m);

void write_map(FILE* f, map m);
void copy_map(map *dest, map *source);

void set_block_at(int x, int y, block_type val, map m);

void set_block_property(int x, int y, block_property p, map m);
void unset_block_property(int x, int y, block_property p, map m);
int get_block_property(int x, int y, block_property p, map m);

block_type get_block_type(int x, int y, map m);

void explode_at(int x, int y, map m);

void destroy_map(map *m);

#endif
