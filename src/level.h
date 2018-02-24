#ifndef BOULDER_LEVEL_H
#define BOULDER_LEVEL_H

#include "stdafx.h"

#include "level.h"
#include "utility.h"

#define GRID_SIZE 8
#define TILE_SIZE 16

#define SCALE_FACTOR 5

#define SPRITE_COUNT 14
enum Sprite {
	BACKGROUND,
	WALL,
	PLAYER_LEFT,
	PLAYER_RIGHT,
	GOAL,
	BOULDER,
	UP_ARROW,
	LEFT_ARROW,
	DOWN_ARROW,
	RIGHT_ARROW,
	UP_STOPPER,
	LEFT_STOPPER,
	DOWN_STOPPER,
	RIGHT_STOPPER,
};

enum Directions {
	D_UP = 0,
	D_LEFT,
	D_DOWN,
	D_RIGHT
};

extern SDL_Texture * sprites[SPRITE_COUNT];

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
	void Create(Vector2i pos);
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
	List<Boulder> boulders;
	//Boulder * boulders;
	//int boulder_num;
	LossCondition loss = NONE;
	int arrows  [GRID_SIZE * GRID_SIZE];
	int stoppers[GRID_SIZE * GRID_SIZE];
	void Alloc();
	void Free();
	bool InBounds(Vector2i pos);
	int BoulderAtPos(Vector2i pos);
	int TriangleAtPos(Vector2i pos);
	bool WallAtPos(Vector2i pos);
	bool Stoppering(Vector2i pos, Vector2i dir);
	void RollBoulder(int i);
	void Update();
	bool MovePlayer(Vector2i epos);
	void Draw(SDL_Renderer * renderer);
	Level * Copy();
};

#endif
