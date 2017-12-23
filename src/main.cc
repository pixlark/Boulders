#include "stdafx.h"

#include "level.h"
#include "loader.h"
#include "utility.h"

TTF_Font * default_font;

struct MovQueue {
	Vector2i mov;
	MovQueue * last;
};

enum GameMode {
	GAME,
	EDITOR
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

/* Average FPS with real-time scaling: 34.975
 * Average FPS with pre-scaling:       36.336
 */

int game_loop(SDL_Surface * screen, SDL_Window * window, bool benchmarking)
{
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

	float  bench_time   = 10.0;
	double bench_acc    = 0.0;
	int    bench_count  = 0;
	
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
		SDL_BlitSurface(sprites[BACKGROUND], NULL, screen, NULL);
		level->Draw(screen);
		// Draw FPS
		{
			char fps_text[16];
			SDL_Color color={0xFF, 0x00, 0x00};
			sprintf(fps_text, "%.2f", 1 / delta_time);
			SDL_Surface * fps_surf = TTF_RenderText_Solid(
				default_font, fps_text, color);
			SDL_BlitSurface(fps_surf, NULL, screen, NULL);
		}
		SDL_UpdateWindowSurface(window);
		update_delta_time();
		if (benchmarking) {
			bench_time  -= delta_time;
			bench_acc   += 1 / delta_time;
			bench_count += 1;
			printf("%2.2f\r", bench_time);
			fflush(stdout);
			if (bench_time <= 0) {
				printf("\nAverage FPS: %f\n", bench_acc / bench_count);
				break;
			}
		}
	}
	return 0;
}

enum EditorMode {
	MODE_WALL,
	MODE_BOULDER,
	MODE_PLAYER,
	MODE_GOAL,
	MODE_ARROW
};

char * editor_mode_names[5] = {
	"wall",
	"boulder",
	"player",
	"goal",
	"arrow"
};

int editor_loop(SDL_Surface * screen, SDL_Window * window)
{
	Level * level = load_level_from_file(find_path("default.lev", "levels"));
	SDL_SetWindowTitle(window, "Level Editor");
	if (level == NULL) {
		printf("Invalid default level file.\n");
		return 1;
	}

	EditorMode mode = MODE_WALL;
	
	init_delta_time();
	SDL_Event event;
	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.scancode) {
				case SDL_SCANCODE_W:
					mode = MODE_WALL;
					break;
				case SDL_SCANCODE_B:
					mode = MODE_BOULDER;
					break;
				case SDL_SCANCODE_P:
					mode = MODE_PLAYER;
					break;
				case SDL_SCANCODE_G:
					mode = MODE_GOAL;
					break;
				case SDL_SCANCODE_A:
					mode = MODE_ARROW;
					break;
				case SDL_SCANCODE_O:
					{
						nfdchar_t * path = NULL;
						nfdresult_t result = NFD_OpenDialog("lev",
							find_path("", "levels"), &path);
						if (result == NFD_OKAY) {
							level = load_level_from_file(path);
						}
						free(path);
					}
					break;
				case SDL_SCANCODE_S:
					{
						nfdchar_t * path = NULL;
						nfdresult_t result = NFD_SaveDialog("lev",
							find_path("", "levels"), &path);
						if (result == NFD_OKAY) {
							save_level_to_file(level, path);
						} else if (result == NFD_ERROR) {
							printf("NFD_ERROR: %s\n", NFD_GetError());
						}
						free(path);
					}
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button != SDL_BUTTON_LEFT) continue;
				int x = event.button.x / (TILE_SIZE * SCALE_FACTOR);
				int y = event.button.y / (TILE_SIZE * SCALE_FACTOR);
				switch (mode) {
				case MODE_WALL:
					level->walls[x + y*GRID_SIZE] = !level->walls[x + y*GRID_SIZE];
					break;
				case MODE_BOULDER:
					{
						int b = level->BoulderAtPos(Vector2i(x, y));
						if (b == -1) {
							Boulder new_boulder;
							new_boulder.Create(Vector2i(x, y));
							level->boulders.Push(new_boulder);
						} else {
							level->boulders.Remove(b);
						}
					}
					break;
				case MODE_PLAYER:
					level->player.pos = Vector2i(x, y);
					level->player.drawable.epos = Vector2i(x * TILE_SIZE, y * TILE_SIZE);
					break;
				case MODE_GOAL:
					level->goal = Vector2i(x, y);
					break;
				case MODE_ARROW:
					level->arrows[x + y*GRID_SIZE] = ((level->arrows[x + y*GRID_SIZE] + 2) % 5) - 1;
					break;
				}
				break;
			}
		}
		SDL_BlitSurface(sprites[BACKGROUND], NULL, screen, NULL);
		level->Draw(screen);
		// Draw FPS | TODO(pixlark): Compress?
		{
			char fps_text[16];
			SDL_Color color={0xFF, 0x00, 0x00};
			sprintf(fps_text, "%.2f", 1 / delta_time);
			SDL_Surface * fps_surf = TTF_RenderText_Solid(
				default_font, fps_text, color);
			SDL_BlitSurface(fps_surf, NULL, screen, NULL);
		}
		// Draw Mode
		{
			char * mode_text = editor_mode_names[mode];
			SDL_Color color={0xFF, 0x00, 0x00};
			SDL_Surface * mode_surf = TTF_RenderText_Solid(
				default_font, mode_text, color);
			SDL_Rect blit_rect = {0, GRID_SIZE * TILE_SIZE * SCALE_FACTOR - 30};
			SDL_BlitSurface(mode_surf, NULL, screen, &blit_rect);
		}
		SDL_UpdateWindowSurface(window);
		update_delta_time();		
	}
	return 0;
}

int main(int argc, char ** argv)
{
	version_check();
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
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

	if (load_sprite(PLAYER_LEFT,  "16\\player.png")      != 0) return 1;
	if (load_sprite(PLAYER_RIGHT, "16\\player.png")      != 0) return 1;
	if (load_sprite(BOULDER,      "16\\boulder.png")     != 0) return 1;
	if (load_sprite(GOAL,         "16\\goal.png")        != 0) return 1;
	if (load_sprite(WALL,         "16\\wall.png")        != 0) return 1;
	if (load_sprite(UP_ARROW,     "16\\up_arrow.png")    != 0) return 1;
	if (load_sprite(LEFT_ARROW,   "16\\left_arrow.png")      != 0) return 1;
	if (load_sprite(DOWN_ARROW,   "16\\down_arrow.png")      != 0) return 1;
	if (load_sprite(RIGHT_ARROW,  "16\\right_arrow.png")     != 0) return 1;
	if (load_sprite(UP_STOPPER,     "16\\up_stopper.png")    != 0) return 1;
	if (load_sprite(LEFT_STOPPER,   "16\\left_stopper.png")  != 0) return 1;
	if (load_sprite(DOWN_STOPPER,   "16\\down_stopper.png")  != 0) return 1;
	if (load_sprite(RIGHT_STOPPER,  "16\\right_stopper.png") != 0) return 1;

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
