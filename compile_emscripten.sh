#!/bin/sh
source /etc/profile.d/emscripten.sh
em++ -std=c++17 -O2 -s ALLOW_MEMORY_GROWTH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s USE_LIBPNG=1 -s USE_SDL_MIXER=2 -Iinclude/ src/*.cpp include/soloud/soloud.o -o bin/pge.js --preload-file ./assets