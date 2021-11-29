// GRR20203927 Gustavo Silveira Frehse
#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *block_name(block_type b) {
	switch (b) {
	case DIRT: return "DIRT";
	case ROCK: return "ROCK";
	case ROCKFORD: return "ROCKFORD";
	case DIAMOND: return "DIAMOND";
	case AIR: return "AIR";
	case BRICK: return "BRICK";
	case STEEL: return "STEEL";
	case DUST: return "DUST";
	case CLOSED_EXIT: return "EXIT";
	case OPEN_EXIT: return "EXIT";
	case NUM_BLOCKS: return "NUM_BLOCKS";
}
return "NOT A BLOCK";
}

void write_block(FILE *f, int x, int y, map m) {
  fprintf(f, "Block of type %s\n", block_name(get_block_type(x, y, m)));

  if (get_block_property(x, y, HAS_GRAVITY, m))
    fprintf(f, "\tHAS_GRAVITY\n");

  if (get_block_property(x, y, IT_COLLIDES, m))
    fprintf(f, "\tIT_COLLIDES\n");

  if (get_block_property(x, y, IS_UNSTABLE, m))
    fprintf(f, "\tIS_UNSTABLE\n");

  if (get_block_property(x, y, IN_MOVEMENT, m))
    fprintf(f, "\tIN_MOVEMENT\n");

  if (get_block_property(x, y, IS_PUSHABLE, m))
    fprintf(f, "\tIS_PUSHABLE\n");

  if (get_block_property(x, y, IS_DIGGABLE, m))
    fprintf(f, "\tIS_DIGGABLE\n");

  if (get_block_property(x, y, HAS_CHANGED, m))
    fprintf(f, "\tHAS_CHANGED\n");

	if (get_block_property(x, y, UNBREAKABLE, m))
    fprintf(f, "\tUNBREAKABLE\n");

	if (get_block_property(x, y, DOES_VANISH, m))
    fprintf(f, "\tDOES_VANISH\n");
}

/**
 * Returns a map from a file handle.
 */
map map_from_file(FILE *f) {
  char s[MAP_MAX_WIDTH + 1];
  char buf[(MAP_MAX_WIDTH + 1) * (MAP_MAX_HEIGHT)];
  buf[0] = '\0';
  int i = 0;

  while (fgets(s, sizeof(s), f) && i++ < MAP_MAX_HEIGHT) {
    strcat(buf, s);
  }

  return map_from_string(buf);
}

block create_block(block_type type) {
	block b;
	b.type = type;
	b.properties = 0x0;
	switch (type) {
	case DIRT:
		b.properties |= IT_COLLIDES | IS_DIGGABLE;
		break;
	case ROCK:
		b.properties |= HAS_GRAVITY | IT_COLLIDES | IS_UNSTABLE | IS_PUSHABLE;
		break;
	case ROCKFORD:
		b.properties |= IT_COLLIDES;
		break;
	case DIAMOND:
	  b.properties |= HAS_GRAVITY | IT_COLLIDES | IS_UNSTABLE | IS_DIGGABLE;
	  break;
	case AIR:
	  break;
	case BRICK:
	  b.properties |= IT_COLLIDES;
	  break;
	case STEEL:
	  b.properties |= IT_COLLIDES | IS_UNSTABLE | UNBREAKABLE;
	  break;
	case DUST:
	  b.properties |= IT_COLLIDES | IS_DIGGABLE | DOES_VANISH;
	  break;
	case CLOSED_EXIT:
	  b.properties |= IT_COLLIDES | UNBREAKABLE;
	  break;
	case OPEN_EXIT:
	  b.properties |= IS_DIGGABLE | IT_COLLIDES | IS_UNSTABLE;
	  break;
	default:
	  fprintf(stderr, "ERROR: received weird block type \"%s\"\n",
	          block_name(type));
	  break;
	}
return b;
}

map map_from_string(char *map_str) {
	map m;
	int total_len = strlen(map_str);
	m.width = 0;
	m.height = 0;
	m.rockford_x = 0;
	m.rockford_y = 0;
	m.exit_x = 0;
	m.exit_y = 0;
	
	// find out the width of the map
	for (m.width = 0; map_str[m.width] != '\n'; m.width++);
	
	// Because total_len counts '\n', we have height-1 lines with '\n'
	// and one without '\n'. Thus:
	// total_len = (width + 1) * ( height - 1) + width
	//   = width * height - width + height - 1 + width
	//   = width * height + height - 1
	//   = (width + 1) * height - 1
	//   => (total_len + 1) / (width + 1) = height
	m.height = (total_len + 1) / (m.width + 1);
	
	fprintf(stderr, "Reading a map with width %d and height %d\n", m.width,
	        m.height);
	
	m.board = calloc(m.width * m.height, sizeof(*(m.board)));
	if (!m.board) {
		fprintf(stderr, "ERROR: Could not allocate memory for the board\n");
		exit(1);
	}
	
	for (int y = 0; y < m.height; y++) {
		for (int x = 0; x < m.width; x++) {
			char c = map_str[y * (m.width + 1) + x]; 
			switch (c) {
			case 'D':
				m.board[y * m.width + x] = create_block(DIRT);
				break;
			case 'O':
				m.board[y * m.width + x] = create_block(ROCK);
				break;
			case 'R':
				m.board[y * m.width + x] = create_block(ROCKFORD);
				m.rockford_x = x;
				m.rockford_y = y;
				break;
			case '*':
				m.needed_diamonds++; // count the amount of diamonds
				m.board[y * m.width + x] = create_block(DIAMOND);
				break;
			case ' ':
				m.board[y * m.width + x] = create_block(AIR);
				break;
			case '#':
				m.board[y * m.width + x] = create_block(BRICK);
				break;
			case '%':
				m.board[y * m.width + x] = create_block(STEEL);
				break;
			case '@':
				m.board[y * m.width + x] = create_block(DUST);
				break;
			case 'E':
				m.board[y * m.width + x] = create_block(CLOSED_EXIT);
				m.exit_x = x;
				m.exit_y = y;
				break;
			case 'e':
				m.board[y * m.width + x] = create_block(OPEN_EXIT);
				m.exit_x = x;
				m.exit_y = y;
			default:
				fprintf(stderr, "Unexpected '%c' (0x%x) while reading a map.\n", c, c);
			}
		}
}

return m;
}

static char block_char(block_type b) {
	switch (b) {
	case DIRT:
		return 'D';
	case ROCK:
		return 'O';
	case ROCKFORD:
		return 'R';
	case DIAMOND:
		return '*';
	case AIR:
		return ' ';
	case BRICK:
		return '#';
	case STEEL:
		return '%';
	case DUST:
		return '@';
	case CLOSED_EXIT:
		return 'E';
	case OPEN_EXIT:
		return 'e';
	default:
		return '?';
	}
}

void open_exit(map m) {
	set_block_at(m.exit_x, m.exit_y, OPEN_EXIT, m);
}

void write_map(FILE *f, map m) {
  for (int y = 0; y < m.height; y++) {
    for (int x = 0; x < m.width; x++) {
      fputc(block_char(get_block_type(x, y, m)), f);
    }
    fputc('\n', f);
  }
}

void copy_map(map *dest, map *source) {
	memcpy(dest, source, sizeof(map));

	dest->board = calloc(dest->width * dest->height, sizeof(block));

	if (!dest->board) {
		fprintf(stderr, "ERROR: Could not allocate memory for second board\n");
		exit(1);
	}

	memcpy(dest->board, source->board, sizeof(block) * dest->width * dest->height);
}

void set_block_at(int x, int y, block_type val, map m) {
  m.board[y * m.width + x] = create_block(val);
}

void set_block_property(int x, int y, block_property p, map m) {
  m.board[y * m.width + x].properties |= p;
}

void unset_block_property(int x, int y, block_property p, map m) {
  m.board[y * m.width + x].properties &= ~p;
}

int get_block_property(int x, int y, block_property p, map m) {
  return m.board[y * m.width + x].properties & p;
}

block_type get_block_type(int x, int y, map m) {
  return m.board[y * m.width + x].type;
}

void explode_at(int x, int y, map m) {
	for (int dx = -1; dx <= 1; dx++) {
		for (int dy = -1; dy <= 1; dy++) {
			if (!get_block_property(x + dx, y + dy, UNBREAKABLE, m)) {
				set_block_at(x + dx, y + dy, DUST, m);
				set_block_property(x + dx, y + dy, HAS_CHANGED, m);
			}
		}
	}
}

void destroy_map(map *m) {
	free(m->board);
}
