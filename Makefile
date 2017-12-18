misc_opts=-std=c++11
src_files=src/main.cc src/loader.cc src/level.cc src/utility.cc
release_name=bin64/boulders
debug_name=debug64/boulders
incl_dirs=-I/usr/include/SDL2
lib_specs=-lSDL2_ttf -lSDL2_mixer -lSDL2_image
warning_opts=-Wno-write-strings

make:
	g++ $(misc_opts) $(src_files) -o $(release_name) $(incl_dirs) $(shell sdl2-config --libs) $(lib_specs) $(warning_opts)
debug:
	g++ -g $(misc_opts) $(src_files) -o $(debug_name) $(incl_dirs) $(shell sdl2-config --libs) $(lib_specs) $(warning_opts)
