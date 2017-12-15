#include <stdio.h>
#include <stdlib.h>

#include "level.h"

void Level::Alloc()
{
	boulders = (Vector2i*) malloc(sizeof(Vector2i) * boulder_num);
	for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
		walls[i] = false;
	}
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

bool Level::WallAtPos(Vector2i pos)
{
	if (walls[pos.x + pos.y * GRID_SIZE]) return true;
	return false;
}

void Level::RollBoulder(int i, Vector2i dir)
{
	while (1) {
		Vector2i next;
		next.x = boulders[i].x + dir.x;
		next.y = boulders[i].y + dir.y;
		if (!InBounds(next)) break;
		if (BoulderAtPos(next) != -1) break;
		if (WallAtPos(next)) break;
		boulders[i].x = next.x;
		boulders[i].y = next.y;
	}
	if (boulders[i].x == goal.x && boulders[i].y == goal.y) {
		loss = WON;
	}
}

bool Level::MovePlayer(Vector2i epos)
{
	Vector2i pos;
	pos.x = (int) (epos.x + 32) / 64;
	pos.y = (int) (epos.y + 32) / 64;
	// Walls
	if (!InBounds(pos)) return false;
	if (WallAtPos(pos)) return false;
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
	player_exact.x = epos.x;
	player_exact.y = epos.y;
	player.x = pos.x;
	player.y = pos.y;
	return true;
}

void Level::Draw(SDL_Surface * screen)
{
	// Draw walls
	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			if (walls[x + y*GRID_SIZE] == true) {
				SDL_Rect wall_rect;
				wall_rect.x = x * 64; wall_rect.y = y * 64;
				SDL_BlitSurface(sprites[WALL], NULL, screen, &wall_rect);
			}
		}
	}
	// Draw goal
	SDL_Rect goal_rect = goal.AsRect();
	goal_rect.x *= 64; goal_rect.y *= 64;
	SDL_BlitSurface(sprites[GOAL], NULL, screen, &goal_rect);
	// Draw boulders
	if (boulders != NULL) {
		for (int i = 0; i < boulder_num; i++) {
			SDL_Rect boulder_rect = boulders[i].AsRect();
			boulder_rect.x *= 64; boulder_rect.y *= 64;
			SDL_BlitSurface(sprites[BOULDER], NULL, screen, &boulder_rect);
		}
	}
	// Draw player
	SDL_Rect player_rect = player_exact.AsRect();
	SDL_BlitSurface(sprites[PLAYER], NULL, screen, &player_rect);
	// Draw lose screen
	switch (loss) {
	case NONE:
		break;
	}
}

