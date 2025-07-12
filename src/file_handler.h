#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#pragma once

#include <assert.h>
#include "cJSON/cJSON.h"
#include "DataStructures/DynamicArray.h"
#include "node.h"
#include "connection.h"
#include "operation.h"
#include <string.h>
#include <stdint.h>
#include <SDL2/SDL.h>

void save_graph_to_json(SimulationState *state);
void load_graph_from_json(SimulationState *state, void *function_data);

#endif // FILEHANDLER_H