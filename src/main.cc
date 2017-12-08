#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include "level.h"
#include "loader.h"
#include "utility.h"

int main()
{
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	SDL_Window * window = SDL_CreateWindow(
		"Boulders",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		GRID_SIZE * 64, GRID_SIZE * 64,
		SDL_WINDOW_SHOWN);
	SDL_Surface * screen = SDL_GetWindowSurface(window);

	sprites[PLAYER]   = SDL_LoadBMP(find_path("player.bmp",   "resources"));
	sprites[BOULDER]  = SDL_LoadBMP(find_path("boulder.bmp",  "resources"));
	sprites[GOAL]     = SDL_LoadBMP(find_path("goal.bmp",     "resources"));
	sprites[TRIANGLE] = SDL_LoadBMP(find_path("triangle.bmp", "resources"));

	SDL_Surface * bg_surf = SDL_CreateRGBSurfaceWithFormat(0, GRID_SIZE * 64, GRID_SIZE * 64, 32, SDL_PIXELFORMAT_RGBA32);
	{
		SDL_Surface * grid_piece = SDL_LoadBMP(find_path("grid.bmp", "resources"));
		for (int i = 0; i < GRID_SIZE; i++) {
			for (int j = 0; j < GRID_SIZE; j++) {
				SDL_Rect pos;
				pos.x = i * 64; pos.y = j * 64;
				SDL_BlitSurface(grid_piece, NULL, bg_surf, &pos);
			}
		}
		SDL_FreeSurface(grid_piece);
	}
	
	// Setup test level
	/*
	Level level;
	level.player = Vector2i(0, 0);
	level.goal = Vector2i(3, 3);
	level.boulders = (Vector2i*) malloc(sizeof(Vector2i) * 2);
	level.boulder_num = 2;
	level.boulders[0] = Vector2i(2, 3);
	level.boulders[1] = Vector2i(2, 5);*/

	Level * level = load_level_from_file(find_path("0.lev", "levels"));
	if (level == NULL) {
		printf("Invalid level file.\n");
		return 1;
	}
	
	SDL_Event event;
	bool running = true;
	while (running) {
		if (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN: {
				Vector2i to_pos = level->player;
				if      (event.key.keysym.scancode == SDL_SCANCODE_W) to_pos.y--;
				else if (event.key.keysym.scancode == SDL_SCANCODE_S) to_pos.y++;
				else if (event.key.keysym.scancode == SDL_SCANCODE_A) to_pos.x--;
				else if (event.key.keysym.scancode == SDL_SCANCODE_D) to_pos.x++;
				level->MovePlayer(to_pos);
			} break;
			}
		}
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		SDL_BlitSurface(bg_surf, NULL, screen, NULL);
		level->Draw(screen);
		SDL_UpdateWindowSurface(window);
	}
	return 0;
}
