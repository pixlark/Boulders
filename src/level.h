#ifndef BOULDER_LEVEL_H
#define BOULDER_LEVEL_H

#include "level.h"
#include "utility.h"

#define GRID_SIZE 8

enum Sprite {
	EMPTY,
	PLAYER,
	GOAL,
	BOULDER,
	TRIANGLE,
	LOST
};

enum LossCondition {
	NONE,
	WON,
	BROKE_TRIANGLE
};

struct Level {
	Vector2i player;
	Vector2i goal;
	Vector2i * boulders = NULL;
	int boulder_num;
	Vector2i * triangles = NULL;
	int triangle_num;
	LossCondition loss = NONE;
	void Alloc();
	void Free();
	bool InBounds(Vector2i pos);
	int BoulderAtPos(Vector2i pos);
	int TriangleAtPos(Vector2i pos);
	void RollBoulder(int index, Vector2i dir);
	bool MovePlayer(Vector2i pos);
	void Draw(SDL_Surface * screen);
};

#endif
