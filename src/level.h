#ifndef BOULDER_LEVEL_H
#define BOULDER_LEVEL_H

#include "stdafx.h"

#include "level.h"
#include "utility.h"

#define GRID_SIZE 8
#define TILE_SIZE 16

#define SCALE_FACTOR 6

#define SPRITE_COUNT 9
enum Sprite {
	WALL,
	PLAYER_LEFT,
	PLAYER_RIGHT,
	GOAL,
	BOULDER,
	UP_ARROW,
	LEFT_ARROW,
	DOWN_ARROW,
	RIGHT_ARROW
};

enum Directions {
	D_UP = 0,
	D_LEFT,
	D_DOWN,
	D_RIGHT
};

extern SDL_Surface * sprites[SPRITE_COUNT];

enum LossCondition {
	NONE,
	WON,
};

struct Drawable {
	Vector2i start;
	Vector2i end;
	float t;
	Vector2i epos;
	bool animating = false;
	void Animate(Vector2i start, Vector2i end);
	bool UpdateAnim(int step);
};

struct Boulder {
	Vector2i pos;
	Vector2i dir;
	Drawable drawable;
};

struct Player {
	Vector2i pos;
	Sprite current_sprite;
	Drawable drawable;
};

struct Level {
	bool walls[GRID_SIZE * GRID_SIZE];
	Player player;
	Vector2i goal;
	Boulder * boulders;
	int boulder_num;
	LossCondition loss = NONE;
	int arrows[GRID_SIZE * GRID_SIZE];
	void Alloc();
	void Free();
	bool InBounds(Vector2i pos);
	int BoulderAtPos(Vector2i pos);
	int TriangleAtPos(Vector2i pos);
	bool WallAtPos(Vector2i pos);
	void RollBoulder(int i);
	void Update();
	bool MovePlayer(Vector2i epos);
	void Draw(SDL_Surface * screen);
	Level * Copy();
};

#endif
