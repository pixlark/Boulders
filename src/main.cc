#include "stdafx.h"

#include "game.h"
#include "editor.h"
#include "level.h"
#include "loader.h"
#include "utility.h"

#define SPRITE_PRE "16\\"
#define SPRITE(val, name) if (load_sprite(val, SPRITE_PRE name) != 0) return 1;

TTF_Font * default_font;

enum GameMode {
	GAME,
	EDITOR
};

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

void scale_sprites(SDL_Surface * screen)
{
	for (int i = 1; i < SPRITE_COUNT; i++) {
		SDL_Surface * tile = sprites[i];
		SDL_Surface * new_sprite = SDL_CreateRGBSurfaceWithFormat(
			0, tile->w * SCALE_FACTOR, tile->h * SCALE_FACTOR,
			32, tile->format->format);
		SDL_Rect blit_rect;
		blit_rect.x = 0;
		blit_rect.y = 0;
		blit_rect.w = tile->w * SCALE_FACTOR;
		blit_rect.h = tile->h * SCALE_FACTOR;
		SDL_BlitScaled(tile, NULL, new_sprite, &blit_rect);
		SDL_FreeSurface(tile);
		sprites[i] = new_sprite;
	}
}

int main(int argc, char ** argv)
{
	version_check();
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG);
	default_font = TTF_OpenFont(find_path("Inconsolata.otf", "resources"), 20);
	
	GameMode game_mode = GAME;
	bool benchmarking = false;
	if (argc > 1) {
		if (strcmp(argv[1], "benchmark") == 0) {
			printf("Benchmarking...\n");
			benchmarking = true;
		} else if (strcmp(argv[1], "editor") == 0) {
			game_mode = EDITOR;
		}
	}
	
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

	// Sprite loading
	SPRITE(PLAYER_LEFT,    "player.png");
	SPRITE(PLAYER_RIGHT,   "player.png");
	SPRITE(BOULDER,        "boulder.png");
	SPRITE(GOAL,           "goal.png");
	SPRITE(WALL,           "wall.png");
	SPRITE(UP_ARROW,       "up_arrow.png");
	SPRITE(LEFT_ARROW,     "left_arrow.png");
	SPRITE(DOWN_ARROW,     "down_arrow.png");
	SPRITE(RIGHT_ARROW,    "right_arrow.png");
	SPRITE(UP_STOPPER,     "up_stopper.png");
	SPRITE(LEFT_STOPPER,   "left_stopper.png");
	SPRITE(DOWN_STOPPER,   "down_stopper.png");
	SPRITE(RIGHT_STOPPER,  "right_stopper.png");

	scale_sprites(screen);
	
	sprites[BACKGROUND] = SDL_CreateRGBSurfaceWithFormat(
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
				SDL_BlitScaled(grid_piece, NULL, sprites[BACKGROUND], &pos);
			}
		}
		SDL_FreeSurface(grid_piece);
	}

	if (game_mode == GAME)
		return game_loop(screen, window, benchmarking);
	else if (game_mode == EDITOR)
		return editor_loop(screen, window);
}
