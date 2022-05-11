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

```
git submodule update --init
mkdir ./build
cmake . -B ./build
cmake --build ./build
```

## License

[MIT](https://github.com/moonburnt/engine/blob/master/LICENSE)
