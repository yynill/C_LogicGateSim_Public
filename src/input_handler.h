#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H
#pragma once

#include "main.h"

void handle_input(SDL_Event *event);

void handle_mouse_button_down(SDL_Event *event);
void handle_mouse_button_up(SDL_Event *event);
void handle_mouse_motion(SDL_Event *event);
void handle_mouse_wheel(SDL_Event *event);

#endif
