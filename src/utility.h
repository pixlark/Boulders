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

template <typename T>
struct List {
	T * arr;
	int len;
	int len_max;
	
	void Alloc();
	void Free();
	List<T> Copy();
	void Push(T p);
	T Pop();
	T& operator[](int indice);
	int Remove(int indice);
	
	void _Grow();
	void _Shrink();
};

template <typename T>
void List<T>::Alloc()
{
	len = 0;
	len_max = 4;
	arr = (T*) malloc(sizeof(T) * len_max);
}

template <typename T>
void List<T>::Free()
{
	free(arr);
}

template <typename T>
List<T> List<T>::Copy()
{
	List<T> ret;
	ret.arr = (T*) malloc(sizeof(T) * len_max);
	ret.len = len;
	ret.len_max = len_max;
	for (int i = 0; i < len; i++) {
		ret.arr[i] = arr[i];
	}
	return ret;
}

template <typename T>
void List<T>::Push(T p)
{
	if (len >= len_max) {
		_Grow();
	}
	arr[len++] = p;
}

template <typename T>
T List<T>::Pop()
{
	assert(len > 0);
	T ret = arr[--len];
	if (len <= len_max / 4 && len_max > 4) {
		_Shrink();
	}
	return ret;
}

template <typename T>
T& List<T>::operator[](int indice)
{
	return arr[indice];
}

template <typename T>
int List<T>::Remove(int indice)
{
	assert(indice < len);
	assert(len > 0);	
	for (int i = indice; i < len - 1; i++) {
		arr[i] = arr[i + 1];
	}
	len--;
	if (len <= len_max / 4 && len_max > 4) {
		_Shrink();
	}
	return 0;
}

template <typename T>
void List<T>::_Grow()
{
	T * new_arr = (T*) malloc(sizeof(T) * len_max * 2);
	for (int i = 0; i < len_max; i++) {
		new_arr[i] = arr[i];
	}
	free(arr);
	len_max *= 2;
	arr = new_arr;
}

template <typename T>
void List<T>::_Shrink()
{
	T * new_arr = (T*) malloc(sizeof(T) * (len_max / 2));
	for (int i = 0; i < (len_max / 2); i++) {
		new_arr[i] = arr[i];
	}
	free(arr);
	len_max /= 2;
	arr = new_arr;
}

void os_path(char * path, int len);
char * find_path(char * name, char * directory);

void update_delta_time();
void init_delta_time();

#endif
