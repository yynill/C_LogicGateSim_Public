#ifndef POINT_H
#define POINT_H
#pragma once

#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>


void point_print(SDL_Point *p);
int point_cross(SDL_Point *a, SDL_Point *b);
SDL_Point point_add(SDL_Point *a, SDL_Point *b);
SDL_Point point_subtract(SDL_Point *a, SDL_Point *b);
SDL_Point point_multiply(SDL_Point *a, double k);
SDL_Point point_divide(SDL_Point *a, double k);
int point_orient(SDL_Point *a, SDL_Point *b, SDL_Point *c);
int segment_intersection(SDL_Point *a, SDL_Point *b, SDL_Point *c, SDL_Point *d, SDL_Point *out);
float distance_to_line_segment(float px, float py, float x1, float y1, float x2, float y2);

#endif // POINT_H