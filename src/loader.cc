#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "loader.h"

static inline void advance_line(FILE * file)
{
	char c;
	while ((c = fgetc(file)) != '\n' && c != EOF);
}

Level * load_level_from_file(char * path)
{
	Level * level = (Level*) malloc(sizeof(Level));
	level->loss = NONE;
	level->boulder_num = 0;
	level->triangle_num = 0;
	FILE * level_file = fopen(path, "r");
	if (level_file == NULL) {
		free(level);
		return NULL;
	}
	char c;
	
	// Count shit
	while ((c = fgetc(level_file)) != EOF) {
		switch(c) {
		case '.':
		case 'X':
		case 'P':
		case '-':
		case '\n':
			break;
		case 'O':
			level->boulder_num++;
			break;
		case 'T':
			level->triangle_num++;
			break;
		default:
			free(level);
			return NULL;
			break; // redundant? whatever
		}
	}

	level->Alloc();
	
	// Do shit
	fseek(level_file, 0, SEEK_SET);
	int boulder_i = 0;
	int triangle_i = 0;
	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			c = fgetc(level_file);
			switch (c) {
			case '.':
				break;
			case 'O':
				level->boulders[boulder_i++] = Vector2i(x, y);
				break;
			case 'X':
				level->goal = Vector2i(x, y);
				break;
			case 'P':
				level->player = Vector2i(x, y);
				break;
			case '-':
				level->walls[x + y * GRID_SIZE] = true;
				break;
			case 'T':
				level->triangles[triangle_i++] = Vector2i(x, y);
				break;
			default:
				level->Free();
				free(level);
				return NULL;
				break;
			}
		}
		advance_line(level_file);
	}
	fclose(level_file);
	return level;
}
