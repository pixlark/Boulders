#ifndef BOULDER_LOADER_H
#define BOULDER_LOADER_H

#include "stdafx.h"

#include "level.h"
#include "utility.h"

char ** load_level_names(char * path, int * count);
Level * load_level_from_file(char * path);

#endif
