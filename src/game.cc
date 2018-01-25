#include "stdafx.h"
#include "game.h"

/* Average FPS with real-time scaling: 34.975
 * Average FPS with pre-scaling:       36.336
 */

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

	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			printf("% 2d", level->stoppers[x + y*GRID_SIZE]);
		}
		printf("\n");
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
