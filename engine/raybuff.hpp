#pragma once

#include "raylib.h"

// Overloads to fuel raylib's C structs with power of C++.

// Equality operator.
bool operator==(const Color& c1, const Color& c2);
bool operator==(const Vector2& v1, const Vector2& v2);
bool operator==(const Rectangle& r1, const Rectangle& r2);

// Addition operator. Iirc these should always return new objects.
Color operator+(const Color& c1, const Color& c2);
Vector2 operator+(const Vector2& v1, const Vector2& v2);

// Subtraction operator.
Color operator-(const Color& c1, const Color& c2);
Vector2 operator-(const Vector2& v1, const Vector2& v2);

// Multiplying operator.
Vector2 operator*(const Vector2 v1, const Vector2 v2);
Vector2 operator*(const Vector2 v1, float val);

// Division operator.
Vector2 operator/(const Vector2 v1, const Vector2 v2);
Vector2 operator/(const Vector2 v1, float val);

// Add in place.
Color& operator+=(Color& c1, const Color& c2);
Vector2& operator+=(Vector2& v1, const Vector2& v2);

// Subtract in place.
Color& operator-=(Color& c1, const Color& c2);
Vector2& operator-=(Vector2& v1, const Vector2& v2);

// Multiply in place.
Vector2& operator*=(Vector2& v1, const Vector2& v2);
Vector2& operator*=(Vector2& v1, float val);

// Divide in place.
Vector2& operator/=(Vector2& v1, const Vector2& v2);
Vector2& operator/=(Vector2& v1, float val);
