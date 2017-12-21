misc_opts=-std=c++11
src_files=src/main.cc src/loader.cc src/level.cc src/utility.cc
release_static_libs=/usr/lib/libnfd.a
debug_static_libs=/usr/lib/libnfd_d.a
release_name=bin64/boulders
debug_name=debug64/boulders
incl_dirs=-I/usr/include/SDL2
lib_specs=-lSDL2_ttf -lSDL2_mixer -lSDL2_image
warning_opts=-Wno-write-strings

make:
	g++ $(misc_opts) $(src_files) $(release_static_libs) -o $(release_name) $(incl_dirs) $(shell pkg-config --cflags --libs gtk+-3.0) $(shell sdl2-config --libs) $(lib_specs) $(warning_opts)
debug:
	g++ -g $(misc_opts) $(src_files) $(debug_static_libs) -o $(debug_name) $(incl_dirs) $(shell pkg-config --cflags --libs gtk+-3.0) $(shell sdl2-config --libs) $(lib_specs) $(warning_opts)
