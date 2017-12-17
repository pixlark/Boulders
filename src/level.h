#ifndef BOULDER_LEVEL_H
#define BOULDER_LEVEL_H

#include "level.h"
#include "utility.h"

#define GRID_SIZE 8

enum Sprite {
	EMPTY,
	WALL,
	PLAYER,
	GOAL,
	BOULDER,
	LOST
};

enum LossCondition {
	NONE,
	WON,
};

struct Drawable {
	Vector2i start;
	Vector2i end;
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
	Drawable drawable;
};

struct Level {
	bool walls[GRID_SIZE * GRID_SIZE];
	Player player;
	Vector2i goal;
	Boulder * boulders;
	int boulder_num;
	LossCondition loss = NONE;
	void Alloc();
	void Free();
	bool InBounds(Vector2i pos);
	int BoulderAtPos(Vector2i pos);
	int TriangleAtPos(Vector2i pos);
	bool WallAtPos(Vector2i pos);
	void Update();
	bool MovePlayer(Vector2i epos);
	void Draw(SDL_Surface * screen);
};

#endif
