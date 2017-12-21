#ifndef BOULDERS_STDAFX_H
#define BOULDERS_STDAFX_H

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nfd.h>

#define SDL_MAIN_HANDLED
#if defined(_WIN32) || defined(_WIN64)
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#endif

#endif
