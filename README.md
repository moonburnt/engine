# Yet Another Game Engine Thingy

## Description

An unnamed WIP raylib-powered engine / extension library to power 2D games.
This is a logical continuation of WGF.

Core ideas:
- Implement basic game-independant functionality (for example: TileMap
template is ok, TileMap with game-specific data is not).
- Only include external dependencies (except raylib) when necessary.

## Pre-requirements

- CMake 3.21+
- clang++ 13.0.1 (mac and linux only)

## Installation

```
git submodule update --init
mkdir ./build
# On mac/linux
CXX=clang++ cmake . -B ./build
# On windows, use this instead
# cmake . -B ./build
cmake --build ./build
```
