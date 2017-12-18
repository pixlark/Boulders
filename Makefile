make:
	g++ -std=c++11 src/main.cc src/loader.cc src/level.cc src/utility.cc \
	-o bin64/boulders -I/usr/include/SDL2 \
	$(shell sdl2-config --libs) \
	-lSDL2_ttf -lSDL2_mixer -lSDL2_image -Wno-write-strings
