#pragma once

#include "raylib.h"
// For clamp
#include <algorithm>

// Overloads to fuel raylib's C structs with power of C++.
// Header-only for now, coz its kinda smol.

// Equality operator.
bool operator==(const Color& c1, const Color& c2) {
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
}

bool operator==(const Vector2& v1, const Vector2& v2) {
    return v1.x == v2.x && v1.y == v2.y;
}

bool operator==(const Rectangle& r1, const Rectangle& r2) {
    return r1.x == r2.x && r1.y == r2.y && r1.width == r2.width && r1.height == r2.height;
}

// Addition operator. Iirc these should always return new objects.
Color operator+(const Color& c1, const Color& c2) {
    return {
        static_cast<unsigned char>(std::clamp((c1.r + c2.r), 0, 255)),
        static_cast<unsigned char>(std::clamp((c1.g + c2.g), 0, 255)),
        static_cast<unsigned char>(std::clamp((c1.b + c2.b), 0, 255)),
        static_cast<unsigned char>(std::clamp((c1.a + c2.a), 0, 255))};
}

Vector2 operator+(const Vector2& v1, const Vector2& v2) {
    return {v1.x + v2.x, v1.y + v2.y};
}

// Subtraction operator.
Color operator-(const Color& c1, const Color& c2) {
    return {
        static_cast<unsigned char>(std::clamp((c1.r - c2.r), 0, 255)),
        static_cast<unsigned char>(std::clamp((c1.g - c2.g), 0, 255)),
        static_cast<unsigned char>(std::clamp((c1.b - c2.b), 0, 255)),
        static_cast<unsigned char>(std::clamp((c1.a - c2.a), 0, 255))};
}

Vector2 operator-(const Vector2& v1, const Vector2& v2) {
    return {v1.x - v2.x, v1.y - v2.y};
}

// Add in place.
Color& operator+=(Color& c1, const Color& c2) {
    c1.r = std::clamp((c1.r + c2.r), 0, 255);
    c1.g = std::clamp((c1.g + c2.g), 0, 255);
    c1.b = std::clamp((c1.b + c2.b), 0, 255);
    c1.a = std::clamp((c1.a + c2.a), 0, 255);

    return c1;
}

Vector2& operator+=(Vector2& v1, const Vector2& v2) {
    v1.x = v1.x + v2.x;
    v1.y = v1.y + v2.y;

    return v1;
}

// Subtract in place.
Color& operator-=(Color& c1, const Color& c2) {
    c1.r = std::clamp((c1.r - c2.r), 0, 255);
    c1.g = std::clamp((c1.g - c2.g), 0, 255);
    c1.b = std::clamp((c1.b - c2.b), 0, 255);
    c1.a = std::clamp((c1.a - c2.a), 0, 255);

    return c1;
}

Vector2& operator-=(Vector2& v1, const Vector2& v2) {
    v1.x = v1.x - v2.x;
    v1.y = v1.y - v2.y;

    return v1;
}
