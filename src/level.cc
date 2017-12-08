#include "level.h"

void Level::Alloc()
{
	boulders = (Vector2i*) malloc(sizeof(Vector2i) * boulder_num);
}

void Level::Free()
{
	free(boulders);
}

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
