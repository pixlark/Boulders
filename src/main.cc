#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
//#include <SDL_image.h> This isn't working for some reason, using BMPs instead :(

#include "utility.h"

#define GRID_SIZE 8

SDL_Surface * sprites[4];
/*
SDL_Surface * player_surf;
SDL_Surface * boulder_surf;
SDL_Surface * goal_surf;
SDL_Surface * triangle_surf;*/
	
struct Vector2f {
	float x;
	float y;
	Vector2f() {}
	Vector2f(float x, float y) { this->x = x; this->y = y; }
};

struct Vector2i {
	int x;
	int y;
	Vector2i() {}
	Vector2i(int x, int y) { this->x = x; this->y = y; }
	SDL_Rect AsRect();
};

SDL_Rect Vector2i::AsRect()
{
	SDL_Rect r;
	r.x = x; r.y = y;
	return r;
}

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	Color() {}
	Color(unsigned char r, unsigned char g, unsigned char b) { this->r = r; this->g = g; this->b = b; }
};

enum Space {
	EMPTY,
	PLAYER,
	GOAL,
	BOULDER,
	TRIANGLE
};

struct Level {
	Vector2i player;
	Vector2i goal;
	Vector2i * boulders = NULL;
	int boulder_num;
	Vector2i * triangles = NULL;
	int triangle_num;
	bool InBounds(Vector2i pos);
	int BoulderAtPos(Vector2i pos);
	void RollBoulder(int index, Vector2i dir);
	bool MovePlayer(Vector2i pos);
	void Draw(SDL_Surface * screen);
};

bool Level::InBounds(Vector2i pos)
{
	if (pos.x < 0 || pos.x >= GRID_SIZE || pos.y < 0 || pos.y >= GRID_SIZE) return false;
	return true;
}

int Level::BoulderAtPos(Vector2i pos)
{
	for (int i = 0; i < boulder_num; i++) {
		if (boulders[i].x == pos.x && boulders[i].y == pos.y) {
			return i;
		}
	}
	return -1;
}

void Level::RollBoulder(int i, Vector2i dir)
{
	while (1) {
		Vector2i next;
		next.x = boulders[i].x + dir.x;
		next.y = boulders[i].y + dir.y;
		if (!InBounds(next)) break;
		if (BoulderAtPos(next) != -1) break;
		boulders[i].x = next.x;
		boulders[i].y = next.y;
	}
}

bool Level::MovePlayer(Vector2i pos)
{
	// Walls
	if (!InBounds(pos)) return false;
	// Boulders
	int b = BoulderAtPos(pos);
	if (b != -1) {
		Vector2i dir;
		dir.x = pos.x - player.x;
		dir.y = pos.y - player.y;
		RollBoulder(b, dir);
		if (BoulderAtPos(pos) == -1) {
			goto move;
		} else {
			return false;
		}
	}
 move:
	player.x = pos.x;
	player.y = pos.y;
	return true;
}

void Level::Draw(SDL_Surface * screen)
{
	SDL_Rect player_rect = player.AsRect();
	player_rect.x *= 64; player_rect.y *= 64;
	SDL_BlitSurface(sprites[PLAYER], NULL, screen, &player_rect);
	SDL_Rect goal_rect = goal.AsRect();
	goal_rect.x *= 64; goal_rect.y *= 64;
	SDL_BlitSurface(sprites[GOAL], NULL, screen, &goal_rect);
	if (boulders != NULL) {
		for (int i = 0; i < boulder_num; i++) {
			SDL_Rect boulder_rect = boulders[i].AsRect();
			boulder_rect.x *= 64; boulder_rect.y *= 64;
			SDL_BlitSurface(sprites[BOULDER], NULL, screen, &boulder_rect);
		}
	}
	if (triangles != NULL) {
		
	}
}

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
	Level level;
	level.player = Vector2i(0, 0);
	level.goal = Vector2i(3, 3);
	level.boulders = (Vector2i*) malloc(sizeof(Vector2i) * 2);
	level.boulder_num = 2;
	level.boulders[0] = Vector2i(2, 3);
	level.boulders[1] = Vector2i(2, 5);
	
	SDL_Event event;
	bool running = true;
	while (running) {
		if (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN: {
				Vector2i to_pos = level.player;
				if      (event.key.keysym.scancode == SDL_SCANCODE_W) to_pos.y--;
				else if (event.key.keysym.scancode == SDL_SCANCODE_S) to_pos.y++;
				else if (event.key.keysym.scancode == SDL_SCANCODE_A) to_pos.x--;
				else if (event.key.keysym.scancode == SDL_SCANCODE_D) to_pos.x++;
				level.MovePlayer(to_pos);
			} break;
			}
		}
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		SDL_BlitSurface(bg_surf, NULL, screen, NULL);
		level.Draw(screen);
		SDL_UpdateWindowSurface(window);
	}
	return 0;
}
