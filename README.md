# Yet Another Game Engine Thingy

## Status

[![Linux + Clang/GCC](https://github.com/moonburnt/engine/actions/workflows/Linux.yml/badge.svg)](https://github.com/moonburnt/engine/actions/workflows/Linux.yml)
[![Windows + MSVC](https://github.com/moonburnt/engine/actions/workflows/Windows%20+%20MSVC.yml/badge.svg)](https://github.com/moonburnt/engine/actions/workflows/Windows%20+%20MSVC.yml)
[![Windows + Clang (MinGW)](https://github.com/moonburnt/engine/actions/workflows/Windows%20+%20Clang%20(MinGW).yml/badge.svg)](https://github.com/moonburnt/engine/actions/workflows/Windows%20+%20Clang%20(MinGW).yml)

## Description

An unnamed WIP raylib-powered engine / extension library to power 2D games.
This is a logical continuation of [WGF](https://github.com/moonburnt/WGF).

Core ideas:
- Implement basic game-independant functionality (for example: TileMap
template is ok, TileMap with game-specific data is not).
- Only include external dependencies (except raylib) when necessary.

## Pre-requirements

- CMake 3.21+

## Installation

### Base

```
git submodule update --init
cmake . -B ./build
cmake --build ./build
```

### Playground

This library includes demo/playground to tinker with. It serves as both usage
example, base project template and practice dummy to test engine changes on.
In order to compile this engine together with its playground, do the following:

```
git submodule update --init
# If you've previously had build directory - delete it
cmake . -B ./build -DCOMPILE_PLAYGROUND=ON
cmake --build ./build
```

To run the playground, cd into ./build/game/ and then run Game executable.

## License

[MIT](https://github.com/moonburnt/engine/blob/master/LICENSE)
