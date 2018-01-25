# Independent
src=src/main.cc src/loader.cc src/level.cc src/utility.cc
other_opts=-std=c++11 -Wno-write-strings -o bin/boulders.exe
dyn_libs=-lSDL2 -lSDL2main -lSDL2_ttf -lSDL2_mixer -lSDL2_image -lnfd

# Windows
win_incl_dirs=-I"G:\.minlib\SDL2-2.0.7\x86_64-w64-mingw32\include\SDL2" \
			  -I"G:\.minlib\SDL2_ttf-2.0.14\x86_64-w64-mingw32\include\SDL2" \
			  -I"G:\.minlib\SDL2_mixer-2.0.2\x86_64-w64-mingw32\include\SDL2" \
			  -I"G:\.minlib\SDL2_image-2.0.2\x86_64-w64-mingw32\include\SDL2" \
			  -I"G:\.libraries\nativefiledialog\src\include"

win_lib_dirs =-L"G:\.minlib\SDL2-2.0.7\x86_64-w64-mingw32\lib" \
			  -L"G:\.minlib\SDL2_ttf-2.0.14\x86_64-w64-mingw32\lib" \
			  -L"G:\.minlib\SDL2_mixer-2.0.2\x86_64-w64-mingw32\lib" \
			  -L"G:\.minlib\SDL2_image-2.0.2\x86_64-w64-mingw32\lib" \
			  -L"G:\.libraries\nativefiledialog\build\lib\Release\x64"
win_opts=-lole32 -luuid

# Nix
nix_incl_dirs=
nix_lib_dirs =
nix_opts=

win_full=$(src) $(win_lib_dirs) $(win_incl_dirs) \
         $(dyn_libs) $(win_opts) $(other_opts)
nix_full=$(src) $(nix_lib_dirs) $(nix_incl_dirs) \
         $(dyn_libs) $(nix_opts) $(other_opts)

win:
	@echo "========================================="
	@echo "== Building Windows Release Executable =="
	@echo "========================================="
	g++ $(win_full)

wing:
	@echo "======================================="
	@echo "== Building Windows Debug Executable =="
	@echo "======================================="
	g++ -g $(win_full)

nix:
	@echo "======================================"
	@echo "== Building *nix Release Executable =="
	@echo "======================================"
	g++ $(nix_full)

nixg:
	@echo "===================================="
	@echo "== Building *nix Debug Executable =="
	@echo "===================================="
	g++ -g $(nix_full)
