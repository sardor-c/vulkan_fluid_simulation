#pragma once

struct Vec2 {
    float x = 0;
    float y = 0;
};

Vec2 add(Vec2 a, Vec2 b);
Vec2 scale(Vec2 v, float s);
float length(Vec2 v);
Vec2 subtract(Vec2 a, Vec2 b);
float dot(Vec2 a, Vec2 b);