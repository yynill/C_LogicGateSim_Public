#ifndef MAIN_H
#define MAIN_H
#pragma once

#include "simulation.h"
#include "input_handler.h"
#include "renderer.h"
#include "performanceMonitor.h"
#include <stdio.h>

#define WINDOW_TITLE "Logic Gate Sim"

#define TARGET_FPS 60
#define FRAME_DELAY (1000 / TARGET_FPS)

#define KEY_UP SDLK_UP
#define KEY_DOWN SDLK_DOWN
#define KEY_LEFT SDLK_LEFT
#define KEY_RIGHT SDLK_RIGHT
#define KEY_A SDLK_z
#define KEY_B SDLK_x
#define KEY_START SDLK_RETURN
#define KEY_SELECT SDLK_SPACE

extern SimulationState *sim_state;

#endif // MAIN_H
