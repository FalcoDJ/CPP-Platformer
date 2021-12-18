#!/bin/sh
# g++ -g -o bin/App src/*.cpp -Iinclude/ -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17
g++ -g -o bin/App src/*.cpp -Iinclude/ -Linclude/soloud/lib/ -Bstatic -static-libgcc -static-libstdc++ -lX11 -lGL -lpthread -lpng -lstdc++fs -lsoloud_static -lSDL2 -ldl