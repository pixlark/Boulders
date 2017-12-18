#include "level.h"

void Level::Alloc()
{
	boulders = (Boulder*) malloc(sizeof(Boulder) * boulder_num);
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
		if (!boulders[i].drawable.animating && boulders[i].pos.x == pos.x && boulders[i].pos.y == pos.y) {
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

bool Level::MovePlayer(Vector2i pos)
{
	if (pos.x == player.pos.x && pos.y == player.pos.y) return false;
	if (player.drawable.animating) return false;
	Vector2i dir;
	dir.x = pos.x - player.pos.x;
	dir.y = pos.y - player.pos.y;
	// Walls
	if (!InBounds(pos)) return false;
	if (WallAtPos(pos)) return false;
	// Boulders
	int bi = BoulderAtPos(pos);
	if (bi != -1) {
		Boulder * b = boulders + bi;
		b->dir = dir;
		RollBoulder(bi);
		/*
		b->drawable.Animate(
			Vector2i(b->pos.x * 64, b->pos.y * 64),
			Vector2i((b->pos.x + b->dir.x) * 64, (b->pos.y + b->dir.y) * 64)
		);
		b->pos.x += b->dir.x;
		b->pos.y += b->dir.y;*/
		
		if (BoulderAtPos(pos) == -1) {
			goto move;
		} else {
			return false;
		}
	}
 move:
	if (dir.x > 0) {
		player.current_sprite = PLAYER_RIGHT;
	}
	if (dir.x < 0) {
		player.current_sprite = PLAYER_LEFT;
	}
	player.drawable.Animate(
		Vector2i(player.pos.x * 64, player.pos.y * 64),
		Vector2i(pos.x * 64, pos.y * 64)
	);
	player.pos.x = pos.x;
	player.pos.y = pos.y;
	return true;
}

void Drawable::Animate(Vector2i start, Vector2i end)
{
	this->start = start;
	this->end = end;
	animating = true;
}

bool Drawable::UpdateAnim(int step)
{
	if (animating) {
		Vector2i dir;
		/* TODO(pixlark) 
		 * We want this is be able to arbitrarily animate to different
		 * positions. At the moment it's limited to lateral moves
		 * since I can't be bothered.
		 *     -Paul T. Sun Dec 17 03:48:09 2017 */
		dir.x = end.x - start.x;
		dir.y = end.y - start.y;
		epos.x += (dir.x / 64) * step;
		epos.y += (dir.y / 64) * step;
		if (epos.x == end.x && epos.y == end.y) {
			printf("finished at pos %d %d\n", epos.x, epos.y);
			animating = false;
			return true;
		}
	}
	return false;
}

void Level::RollBoulder(int i)
{
	Boulder * b = boulders + i;
	Vector2i next;
	next.x = b->pos.x + b->dir.x;
	next.y = b->pos.y + b->dir.y;
	if (InBounds(next) &&
		!WallAtPos(next) &&
		BoulderAtPos(next) == -1) {
		b->drawable.Animate(
			Vector2i(b->pos.x * 64, b->pos.y * 64),
			Vector2i((b->pos.x + b->dir.x) * 64, (b->pos.y + b->dir.y) * 64)
		);
		b->pos.x += b->dir.x;
		b->pos.y += b->dir.y;
	}
}

void Level::Update()
{
	for (int i = 0; i < boulder_num; i++) {
		Boulder * b = boulders + i;
		if (b->drawable.UpdateAnim(2)) {
			RollBoulder(i);
		}
		if (!b->drawable.animating) {
			if (goal.x == b->pos.x && goal.y == b->pos.y) {
				loss = WON;
			}
		}
	}
	player.drawable.UpdateAnim(2);
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
			SDL_Rect boulder_rect = boulders[i].drawable.epos.AsRect();
			SDL_BlitSurface(sprites[BOULDER], NULL, screen, &boulder_rect);
		}
	}
	// Draw player
	//SDL_Rect player_rect = player_exact.AsRect();
	SDL_Rect player_rect = player.drawable.epos.AsRect();
	SDL_BlitSurface(sprites[player.current_sprite], NULL, screen, &player_rect);
	// Draw lose screen
	switch (loss) {
	case NONE:
		break;
	}
}

