#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

#include "level.h"
#include "loader.h"
#include "utility.h"

TTF_Font * default_font;

#define LEVEL_COUNT 4

char * levels[LEVEL_COUNT] = {
	"middle-goal.lev",
	"basic-no-tri.lev",
	"basic-tri.lev",
	"gateway.lev"
};
int lev_i = 0;

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

int main()
{
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	default_font = TTF_OpenFont(find_path("Inconsolata.otf", "resources"), 20);

	SDL_Window * window = SDL_CreateWindow(
		"Boulders",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		GRID_SIZE * 64, GRID_SIZE * 64,
		SDL_WINDOW_SHOWN);
	SDL_Surface * screen = SDL_GetWindowSurface(window);

	sprites[PLAYER_LEFT]    = IMG_Load(find_path("player_left.png",  "resources"));
	sprites[PLAYER_RIGHT]   = IMG_Load(find_path("player_right.png", "resources"));
	sprites[BOULDER]  = IMG_Load(find_path("boulder.png", "resources"));
	sprites[GOAL]     = IMG_Load(find_path("goal.png",    "resources"));
	sprites[WALL]     = IMG_Load(find_path("wall.png",    "resources"));
	{
		SDL_Color tc;
		tc.r = 0x00; tc.g = 0x00; tc.b = 0x00;
		sprites[LOST] = TTF_RenderText_Blended(
			default_font,
			"You broke a triangle. R to restart.",
			tc);
	}

	SDL_Surface * bg_surf = SDL_CreateRGBSurfaceWithFormat(0, GRID_SIZE * 64, GRID_SIZE * 64, 32, SDL_PIXELFORMAT_RGBA32);
	{
		SDL_Surface * grid_piece = IMG_Load(find_path("bg.png", "resources"));
		for (int i = 0; i < GRID_SIZE; i++) {
			for (int j = 0; j < GRID_SIZE; j++) {
				SDL_Rect pos;
				pos.x = i * 64; pos.y = j * 64;
				SDL_BlitSurface(grid_piece, NULL, bg_surf, &pos);
			}
		}
		SDL_FreeSurface(grid_piece);
	}

	Level * level = load_level_from_file(find_path(levels[lev_i], "levels"));
	if (level == NULL) {
		printf("Invalid level file.\n");
		return 1;
	}

	MovQueue * mov_queue = NULL;
	
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
					level = load_level_from_file(
						find_path(levels[lev_i], "levels"));
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
			if (lev_i >= LEVEL_COUNT) return 0;
			level->Free();
			free(level);
			level = load_level_from_file(
				find_path(levels[lev_i], "levels"));
		}
		level->Update();
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		SDL_BlitSurface(bg_surf, NULL, screen, NULL);
		level->Draw(screen);
		SDL_UpdateWindowSurface(window);
	}
	return 0;
}
