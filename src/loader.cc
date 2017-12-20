#include "loader.h"

static inline void advance_line(FILE * file)
{
	char c;
	while ((c = fgetc(file)) != '\n' && c != EOF);
}

char ** load_level_names(char * path, int * count)
{
	FILE * index_file = fopen(path, "r");
	fscanf(index_file, "%d", count);
	fgetc(index_file);
	char ** names = (char**) malloc(sizeof(char*) * *count);
	for (int i = 0; i < *count; i++) {
		names[i] = (char*) malloc(sizeof(char) * 512);
		fscanf(index_file, "%s", names[i]);
	}
	return names;
}

Level * load_level_from_file(char * path)
{
	Level * level = (Level*) malloc(sizeof(Level));
	level->loss = NONE;
	level->boulder_num = 0;
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
	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			c = fgetc(level_file);
			switch (c) {
			case '.':
				break;
			case 'O':
				level->boulders[boulder_i].pos = Vector2i(x, y);
				level->boulders[boulder_i].drawable.epos = Vector2i(x * TILE_SIZE, y * TILE_SIZE);
				level->boulders[boulder_i].drawable.animating = false;
				boulder_i++;
				break;
			case 'X':
				level->goal = Vector2i(x, y);
				break;
			case 'P':
				level->player.pos = Vector2i(x, y);
				level->player.drawable.epos = Vector2i(x * TILE_SIZE, y * TILE_SIZE);
				level->player.drawable.animating = false;
				level->player.current_sprite = PLAYER_LEFT;
				break;
			case '-':
				level->walls[x + y * GRID_SIZE] = true;
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
