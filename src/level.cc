#include "level.h"

SDL_Surface * sprites[SPRITE_COUNT];

void Level::Alloc()
{
	boulders.Alloc();
	for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
		walls[i] = false;
		arrows[i] = -1;
	}
	loss = NONE;
}

void Level::Free()
{
	boulders.Free();
}

bool Level::InBounds(Vector2i pos)
{
	if (pos.x < 0 || pos.x >= GRID_SIZE || pos.y < 0 || pos.y >= GRID_SIZE) return false;
	return true;
}

int Level::BoulderAtPos(Vector2i pos)
{
	for (int i = 0; i < boulders.len; i++) {
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
		Boulder * b = &(boulders[bi]);
		b->dir = dir;
		RollBoulder(bi);
		/*
		b->drawable.Animate(
			Vector2i(b->pos.x * TILE_SIZE, b->pos.y * TILE_SIZE),
			Vector2i((b->pos.x + b->dir.x) * TILE_SIZE, (b->pos.y + b->dir.y) * TILE_SIZE)
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
		Vector2i(player.pos.x * TILE_SIZE, player.pos.y * TILE_SIZE),
		Vector2i(pos.x * TILE_SIZE, pos.y * TILE_SIZE)
	);
	player.pos.x = pos.x;
	player.pos.y = pos.y;
	return true;
}

void Drawable::Animate(Vector2i start, Vector2i end)
{
	this->start = start;
	this->end = end;
	this->t = 0;
	animating = true;
}

bool Drawable::UpdateAnim(int step)
{
	if (animating) {
		Vector2i dir;
		t += delta_time;
		epos.x = start.x + (end.x - start.x) * t * step;
		epos.y = start.y + (end.y - start.y) * t * step;
		if (t >= 1.0 / step) {
			epos.x = end.x;
			epos.y = end.y;
			animating = false;
			return true;
		}
	}
	return false;
}

void Level::RollBoulder(int i)
{
	Boulder * b = &(boulders[i]);
	if (arrows[b->pos.x + b->pos.y*GRID_SIZE] != -1) {
		switch (arrows[b->pos.x + b->pos.y*GRID_SIZE]) {
		case D_UP:
			b->dir.x = 0;
			b->dir.y = -1;
			break;
		case D_LEFT:
			b->dir.x = -1;
			b->dir.y = 0;
			break;
		case D_DOWN:
			b->dir.x = 0;
			b->dir.y = 1;
			break;
		case D_RIGHT:
			b->dir.x = 1;
			b->dir.y = 0;
			break;
		}
	}
	Vector2i next;
	next.x = b->pos.x + b->dir.x;
	next.y = b->pos.y + b->dir.y;
	if (InBounds(next) &&
		!WallAtPos(next) &&
		BoulderAtPos(next) == -1) {
		b->drawable.Animate(
			Vector2i(b->pos.x * TILE_SIZE, b->pos.y * TILE_SIZE),
			Vector2i((b->pos.x + b->dir.x) * TILE_SIZE, (b->pos.y + b->dir.y) * TILE_SIZE)
		);
		b->pos.x += b->dir.x;
		b->pos.y += b->dir.y;
	}
}

void Level::Update()
{
	for (int i = 0; i < boulders.len; i++) {
		Boulder * b = &(boulders[i]);
		if (b->drawable.UpdateAnim(5)) {
			RollBoulder(i);
		}
		if (!b->drawable.animating) {
			if (goal.x == b->pos.x && goal.y == b->pos.y) {
				loss = WON;
			}
		}
	}
	player.drawable.UpdateAnim(5);
}

// Assumes you've scaled your coordinates to real position by tile size
void draw_tile(SDL_Surface * screen, SDL_Surface * tile, int x, int y)
{
	SDL_Rect blit_rect;
	blit_rect.x = x * SCALE_FACTOR; blit_rect.y = y * SCALE_FACTOR;
	SDL_BlitSurface(tile, NULL, screen, &blit_rect);
}

void Level::Draw(SDL_Surface * screen)
{
	// Draw walls and arrows
	for (int y = 0; y < GRID_SIZE; y++) {
		for (int x = 0; x < GRID_SIZE; x++) {
			if (arrows[x + y*GRID_SIZE] != -1) {
				switch (arrows[x + y*GRID_SIZE]) {
				case D_UP:
					draw_tile(screen, sprites[UP_ARROW], x * TILE_SIZE, y * TILE_SIZE);
					break;
				case D_LEFT:
					draw_tile(screen, sprites[LEFT_ARROW], x * TILE_SIZE, y * TILE_SIZE);
					break;
				case D_DOWN:
					draw_tile(screen, sprites[DOWN_ARROW], x * TILE_SIZE, y * TILE_SIZE);
					break;
				case D_RIGHT:
					draw_tile(screen, sprites[RIGHT_ARROW], x * TILE_SIZE, y * TILE_SIZE);
					break;
				}
			}
			if (walls[x + y*GRID_SIZE] == true) {
				draw_tile(screen, sprites[WALL], x * TILE_SIZE, y * TILE_SIZE);
			}
		}
	}
	// Draw goal
	draw_tile(screen, sprites[GOAL], goal.x * TILE_SIZE, goal.y * TILE_SIZE);
	// Draw boulders
	for (int i = 0; i < boulders.len; i++) {
		draw_tile(screen, sprites[BOULDER], boulders[i].drawable.epos.x, boulders[i].drawable.epos.y);
	}
	// Draw player
	draw_tile(screen, sprites[player.current_sprite], player.drawable.epos.x, player.drawable.epos.y);
}

Level * Level::Copy()
{
	Level * ret = (Level*) malloc(sizeof(Level));
	for (int i = 0; i < GRID_SIZE * GRID_SIZE; i++) {
		ret->walls[i] = walls[i];
		ret->arrows[i] = arrows[i];
	}
	ret->player = player;
	ret->goal = goal;
	ret->boulders = boulders.Copy();
	ret->loss = loss;
	return ret;
}
