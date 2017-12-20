#include "stdafx.h"

#include "level.h"
#include "loader.h"
#include "utility.h"

TTF_Font * default_font;

struct MovQueue {
	Vector2i mov;
	MovQueue * last;
};

void mov_queue_push(MovQueue ** queue, Vector2i mov)
{
	MovQueue * next = (MovQueue*) malloc(sizeof(MovQueue));
	next->mov  = mov;
	next->last = *queue;
	*queue = next;
}

Vector2i mov_queue_pop(MovQueue ** queue)
{
	Vector2i ret = (*queue)->mov;
	*queue = (*queue)->last;
	return ret;
}

void version_check()
{
	SDL_version compiled;
	SDL_version linked;
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	printf("Compiled against SDL version %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
	printf("Linked against SDL version %d.%d.%d\n", linked.major, linked.minor, linked.patch);
}

int load_sprite(Sprite index, char * name)
{
	sprites[index] = IMG_Load(find_path(name, "resources"));
	if (sprites[index] == NULL) {
		fprintf(stderr, "Image '%s' did not load correctly.\n", name);
		return 1;
	}
	return 0;
}

int main()
{
	version_check();
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	default_font = TTF_OpenFont(find_path("Inconsolata.otf", "resources"), 20);

	SDL_Window * window = SDL_CreateWindow(
		"Boulders",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		GRID_SIZE * TILE_SIZE * SCALE_FACTOR, GRID_SIZE * TILE_SIZE * SCALE_FACTOR,
		SDL_WINDOW_SHOWN);
	if (window == NULL) {
		fprintf(stderr, "Could not create window.\n");
		return 1;
	}
	SDL_Surface * screen = SDL_GetWindowSurface(window);
	if (screen == NULL) {
		fprintf(stderr, "Count not retrieve screen surface.\n");
		return 1;
	}

	if (load_sprite(PLAYER_LEFT,  "16\\player.png")  != 0) return 1;
	if (load_sprite(PLAYER_RIGHT, "16\\player.png") != 0) return 1;
	if (load_sprite(BOULDER,      "16\\boulder.png")      != 0) return 1;
	if (load_sprite(GOAL,         "16\\goal.png")         != 0) return 1;
	if (load_sprite(WALL,         "16\\wall.png")         != 0) return 1;
	if (load_sprite(UP_ARROW, "16\\up_arrow.png") != 0) return 1;
	if (load_sprite(LEFT_ARROW, "16\\left_arrow.png") != 0) return 1;
	if (load_sprite(DOWN_ARROW, "16\\down_arrow.png") != 0) return 1;
	if (load_sprite(RIGHT_ARROW, "16\\right_arrow.png") != 0) return 1;

	SDL_Surface * bg_surf = SDL_CreateRGBSurfaceWithFormat(
		0, GRID_SIZE * TILE_SIZE * SCALE_FACTOR, GRID_SIZE * TILE_SIZE * SCALE_FACTOR, 32, SDL_PIXELFORMAT_RGBA32);
	{
		SDL_Surface * grid_piece = IMG_Load(find_path("16\\ice.png", "resources"));
		if (grid_piece == NULL) {
			fprintf(stderr, "Could not load background tile image.\n");
			return 1;
		}
		for (int i = 0; i < GRID_SIZE; i++) {
			for (int j = 0; j < GRID_SIZE; j++) {
				SDL_Rect pos;
				pos.x = i * TILE_SIZE * SCALE_FACTOR; pos.y = j * TILE_SIZE * SCALE_FACTOR;
				pos.w = TILE_SIZE * SCALE_FACTOR;
				pos.h = TILE_SIZE * SCALE_FACTOR;
				SDL_BlitScaled(grid_piece, NULL, bg_surf, &pos);
			}
		}
		SDL_FreeSurface(grid_piece);
	}

	int level_count;
	int lev_i = 0;
	char ** level_names = load_level_names(find_path("index", "levels"), &level_count);
	Level ** levels = (Level**) malloc(sizeof(Level*) * level_count);
	
	for (int i = 0; i < level_count; i++) {
		levels[i] = load_level_from_file(find_path(level_names[i], "levels"));
	}

	Level * level = levels[lev_i]->Copy();
	SDL_SetWindowTitle(window, level_names[lev_i]);
	if (level == NULL) {
		printf("Invalid level file.\n");
		return 1;
	}

	/*
	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			printf("% 2d ", level->arrows[x + y*GRID_SIZE]);
		}
		printf("\n");
		}*/
	
	MovQueue * mov_queue = NULL;

	init_delta_time();
	SDL_Event event;
	bool running = true;
	while (running) {
		Vector2i cdir(0, 0);
		if (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN: {
				if (event.key.repeat) continue; 
				switch(event.key.keysym.scancode) {
				case SDL_SCANCODE_R:
					level->Free();
					free(level);
					level = levels[lev_i]->Copy();
					break;
				}
				// MOVEMENT
				if (level->loss == NONE) {
					switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_W:
						cdir.y = -1;
						break;
					case SDL_SCANCODE_S:
						cdir.y = 1;
						break;
					case SDL_SCANCODE_A:
						cdir.x = -1;
						break;
					case SDL_SCANCODE_D:
						cdir.x = 1;
						break;
					}
				}
			} break;
			case SDL_KEYUP: {
			} break;
			}
		}
		if (cdir.x != 0 || cdir.y != 0) {
			mov_queue_push(&mov_queue, cdir);
		}
		if (!level->player.drawable.animating && mov_queue != NULL) {
			Vector2i dir;
			dir = mov_queue_pop(&mov_queue);
			Vector2i to_pos = level->player.pos;
			to_pos.x += dir.x;
			to_pos.y += dir.y;
			level->MovePlayer(to_pos);
		}
		if (level->loss == WON) {
			lev_i++;
			if (lev_i >= level_count) return 0;
			level->Free();
			free(level);
			level = levels[lev_i]->Copy();
			SDL_SetWindowTitle(window, level_names[lev_i]);
		}
		level->Update();
		//SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		SDL_BlitSurface(bg_surf, NULL, screen, NULL);
		level->Draw(screen);
		SDL_UpdateWindowSurface(window);
		update_delta_time();
	}
	return 0;
}
