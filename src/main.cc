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

TTF_Font * default_font;

char * levels[2] = {
	"0.lev",
	"1.lev"
};
int lev_i = 0;

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

	sprites[PLAYER]   = SDL_LoadBMP(find_path("player.bmp",   "resources"));
	sprites[BOULDER]  = SDL_LoadBMP(find_path("boulder.bmp",  "resources"));
	sprites[GOAL]     = SDL_LoadBMP(find_path("goal.bmp",     "resources"));
	sprites[TRIANGLE] = SDL_LoadBMP(find_path("triangle.bmp", "resources"));
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

	Level * level = load_level_from_file(find_path(levels[lev_i], "levels"));
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
				switch(event.key.keysym.scancode) {
				case SDL_SCANCODE_R:
					level->Free();
					free(level);
					level = load_level_from_file(
						find_path(levels[lev_i], "levels"));
					break;
				}
				// MOVEMENT
				Vector2i to_pos = level->player;
				if (level->loss == NONE) {
					switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_W:
						to_pos.y--;
						break;
					case SDL_SCANCODE_S:
						to_pos.y++;
						break;
					case SDL_SCANCODE_A:
						to_pos.x--;
						break;
					case SDL_SCANCODE_D:
						to_pos.x++;
						break;
					}
				}
				level->MovePlayer(to_pos);
				if (level->loss == WON) {
					lev_i++;
					level->Free();
					free(level);
					level = load_level_from_file(
						find_path(levels[lev_i], "levels"));
				}
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
