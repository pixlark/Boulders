#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "utility.h"

SDL_Surface * sprites[4];

SDL_Rect Vector2i::AsRect()
{
	SDL_Rect r;
	r.x = x; r.y = y;
	return r;
}

void os_path(char * path, int len)
{
	for (int i = 0; i < len; i++) {
		if (path[i] == '/' || path[i] == '\\') {
#if defined(_WIN32) || defined(_WIN64)
			path[i] = '\\';
#else
			path[i] = '/';
#endif			
		}
	}
}

char * find_path(char * name, char * directory)
{
	char * buffer = (char*) malloc(512);
#if defined(_WIN32) || defined(_WIN64)
	int rlen = GetModuleFileName(NULL, buffer, 512);
	if (rlen == 0) return NULL;
	char * ls = strrchr(buffer, '\\');
	*(ls + 1) = '\0'; // truncate exe name
	strcat(buffer, "..\\");
#else
	int rlen = readlink("/proc/self/exe", buffer, 511);
	if (rlen == -1) return NULL;
	buffer[rlen] = '\0';
	char * ls = strrchr(buffer, '/');
	*(ls + 1) = '\0'; // truncate exe name
	strcat(buffer, "../");
#endif
	strcat(buffer, directory);
	strcat(buffer, "/");
	strcat(buffer, name);
	os_path(buffer, strlen(buffer));
	return buffer;
}
