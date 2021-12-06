#!/bin/sh
g++ -g -o bin/App src/*.cpp -Iinclude/ -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17