#include "stdafx.h"
#include "editor.h"

enum EditorMode {
	MODE_WALL,
	MODE_BOULDER,
	MODE_PLAYER,
	MODE_GOAL,
	MODE_ARROW,
	MODE_STOPPER
};

char * editor_mode_names[] = {
	"wall",
	"boulder",
	"player",
	"goal",
	"arrow",
	"stopper"
};

int editor_loop(SDL_Renderer * renderer, SDL_Window * window)
{
	Level * level = load_level_from_file(find_path("default.lev", "levels"));
	SDL_SetWindowTitle(window, "...");
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
				case SDL_SCANCODE_T:
					mode = MODE_STOPPER;
					break;
				case SDL_SCANCODE_O:
					{
						nfdchar_t * path = NULL;
						nfdresult_t result = NFD_OpenDialog("lev",
							find_path("", "levels"), &path);
						if (result == NFD_OKAY) {
							level = load_level_from_file(path);
						}
						SDL_SetWindowTitle(window, path);
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
							SDL_SetWindowTitle(window, path);
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
				case MODE_STOPPER:
					level->stoppers[x + y*GRID_SIZE] = ((level->stoppers[x + y*GRID_SIZE] + 2) % 5) - 1;
					break;
				}
				break;
			}
		}
		SDL_RenderCopy(renderer, sprites[BACKGROUND], NULL, NULL);
		level->Draw(renderer);
		// Draw FPS | TODO(pixlark): Compress?
		{
			char fps_text[16];
			SDL_Color color={0xFF, 0x00, 0x00};
			sprintf(fps_text, "%.2f", 1 / delta_time);
			SDL_Surface * fps_surf = TTF_RenderText_Solid(
				default_font, fps_text, color);
			SDL_Texture * fps_tex = create_tex_with_access(renderer, fps_surf, SDL_TEXTUREACCESS_STREAMING);
			SDL_RenderCopy(renderer, fps_tex, NULL, NULL);
			SDL_FreeSurface(fps_surf);
			SDL_DestroyTexture(fps_tex);
		}
		// Draw Mode
		{
			char * mode_text = editor_mode_names[mode];
			SDL_Color color={0xFF, 0x00, 0x00};
			SDL_Surface * mode_surf = TTF_RenderText_Solid(
				default_font, mode_text, color);
			SDL_Texture * mode_tex = create_tex_with_access(renderer, mode_surf, SDL_TEXTUREACCESS_STREAMING);
			SDL_Rect blit_rect = {0, GRID_SIZE * TILE_SIZE * SCALE_FACTOR - 30};
			SDL_RenderCopy(renderer, mode_tex, NULL, &blit_rect);
			SDL_FreeSurface(mode_surf);
			SDL_DestroyTexture(mode_tex);
		}
		SDL_RenderPresent(renderer);
		update_delta_time();		
	}
	return 0;
}
