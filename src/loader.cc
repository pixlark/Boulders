#include "loader.h"

char arrow_chars[4] = {'^', '<', 'v', '>'};

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

Level * save_level_to_file(Level * level, char * path)
{
	FILE * level_file = fopen(path, "w");
	// Write in background
	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			fputc('.', level_file);
		}
		fputc('\n', level_file);
	}
	fseek(level_file, 0, SEEK_SET);
	// Write in boulders
	for (int i = 0; i < level->boulders.len; i++) {
		fseek(level_file, level->boulders[i].pos.x + level->boulders[i].pos.y * (GRID_SIZE + 1), SEEK_SET);
		fputc('O', level_file);
	}
	// Write in walls
	fseek(level_file, 0, SEEK_SET);
	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			if (level->walls[x + y*GRID_SIZE]) fputc('-', level_file);
			else fseek(level_file, 1, SEEK_CUR);
		}
		fseek(level_file, 1, SEEK_CUR);
	}
	// Write in arrows
	fseek(level_file, 0, SEEK_SET);
	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			if (level->arrows[x + y*GRID_SIZE] != -1) fprintf(level_file, "%c", arrow_chars[level->arrows[x + y*GRID_SIZE]]);
			else fseek(level_file, 1, SEEK_CUR);
		}
		fseek(level_file, 1, SEEK_CUR);
	}
	// Write in player
	fseek(level_file, level->player.pos.x + level->player.pos.y * (GRID_SIZE + 1), SEEK_SET);
	fputc('P', level_file);
	// Write in goal
	fseek(level_file, level->goal.x + level->goal.y * (GRID_SIZE + 1), SEEK_SET);
	fputc('X', level_file);
	fclose(level_file);
}

Level * load_level_from_file(char * path)
{
	Level * level = (Level*) malloc(sizeof(Level));
	FILE * level_file = fopen(path, "r");
	if (level_file == NULL) {
		free(level);
		return NULL;
	}
	char c;
	
	// Count stuff
	while ((c = fgetc(level_file)) != EOF) {
		switch(c) {
		case '.':
		case 'X':
		case 'P':
		case '-':
		case '>':
		case '<':
		case '^':
		case 'v':
		case 'O':
		case '\n':
			break;
		default:
			free(level);
			return NULL;
			break; // redundant? whatever
		}
	}

	level->Alloc();
	
	// Do stuff
	fseek(level_file, 0, SEEK_SET);
	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			c = fgetc(level_file);
			switch (c) {
			case '.':
				break;
			case 'O':
				{
					Boulder new_boulder;
					new_boulder.Create(Vector2i(x, y));
					level->boulders.Push(new_boulder);
				}
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
			case '>':
				level->arrows[x + y * GRID_SIZE] = D_RIGHT;
				break;
			case '<':
				level->arrows[x + y * GRID_SIZE] = D_LEFT;
				break;
			case '^':
				level->arrows[x + y * GRID_SIZE] = D_UP;
				break;
			case 'v':
				level->arrows[x + y * GRID_SIZE] = D_DOWN;
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
