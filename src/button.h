#ifndef Button_H
#define Button_H
#pragma once

#include <SDL2/SDL.h>
#include "simulation.h"

typedef struct SimulationState SimulationState;

typedef struct Button
{
    SDL_Rect rect;
    char *name;
    void *function_data;
    void (*on_press)(void *function_data);
} Button;

Button *create_button(SDL_Rect rect, char *name, void *function_data, void (*on_press)(void*));

#endif // Button_H
