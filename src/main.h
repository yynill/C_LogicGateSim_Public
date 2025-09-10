#ifndef MAIN_H
#define MAIN_H
#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <assert.h>
#include <math.h>
#include <dirent.h>
#include <time.h>
#include <sys/time.h>

#include "DataStructures/DynamicArray.h"
#include "DataStructures/Queue.h"
#include "cJSON/cJSON.h"
#include "coordinate_system.h"
#include "operation.h"
#include "button.h"
#include "popup_state.h"
#include "node.h"
#include "pin.h"
#include "connection.h"
#include "file_handler.h"
#include "renderer.h"
#include "performanceMonitor.h"
#include "input_handler.h"
#include "simulation.h"

typedef struct SimulationState SimulationState;
extern SimulationState *sim_state;

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

#endif // MAIN_H
