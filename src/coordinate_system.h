#ifndef COORDINATE_SYSTEM_H
#define COORDINATE_SYSTEM_H

#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Point SDL_Point;

typedef struct Float_Rect {
    float x, y;
    float w, h;
} Float_Rect;

typedef struct Float_Point   {
    float x, y;
} Float_Point;

SDL_Rect float_rect_to_sdl_rect(Float_Rect *rect);
Float_Rect sdl_rect_to_float_rect(SDL_Rect *rect);

void point_print(SDL_Point *p);
int point_cross(SDL_Point *a, SDL_Point *b);
SDL_Point point_add(SDL_Point *a, SDL_Point *b);
SDL_Point point_subtract(SDL_Point *a, SDL_Point *b);
SDL_Point point_multiply(SDL_Point *a, double k);
SDL_Point point_divide(SDL_Point *a, double k);
int point_orient(SDL_Point *a, SDL_Point *b, SDL_Point *c);
int segment_intersection(SDL_Point *a, SDL_Point *b, SDL_Point *c, SDL_Point *d, SDL_Point *out);
float distance_to_line_segment(float px, float py, float x1, float y1, float x2, float y2);

#endif // COORDINATE_SYSTEM_H
