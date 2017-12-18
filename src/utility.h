#ifndef BOULDER_UTILITY_H
#define BOULDER_UTILITY_H

#include "stdafx.h"

extern double delta_time;
extern uint64_t last_count;

struct Vector2f {
	float x;
	float y;
	Vector2f() {}
	Vector2f(float x, float y) { this->x = x; this->y = y; }
};

struct Vector2i {
	int x;
	int y;
	Vector2i() {}
	Vector2i(int x, int y) { this->x = x; this->y = y; }
	SDL_Rect AsRect();
};

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	Color() {}
	Color(unsigned char r, unsigned char g, unsigned char b) { this->r = r; this->g = g; this->b = b; }
};

void os_path(char * path, int len);
char * find_path(char * name, char * directory);

void update_delta_time();
void init_delta_time();

#endif
