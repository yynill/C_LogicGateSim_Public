#ifndef NODE_H
#define NODE_H
#pragma once

#include <SDL2/SDL.h>
#include <assert.h>
#include "DataStructures/DynamicArray.h"
#include "pin.h"
#include "button.h"
#include "operation.h"
#include "renderer.h"
#include "connection.h"

typedef struct SimulationState SimulationState;
typedef struct Button Button;

typedef struct Node {
    char *name;
    int is_expanded;
    SDL_Rect rect;

    DynamicArray *inputs;  // pins
    DynamicArray *outputs; // pins
    Operation *operation;

    DynamicArray *sub_nodes;
    DynamicArray *sub_connections;
    struct Node *parent;
    SDL_Rect outline_rect;
    Button *close_btn;
} Node;

// functions
void print_node(Node *node);
void run_node(Node *node);
void move_group_node_pins(Node *node);
void reshape_outline_box(Node *node);

Node *create_node(DynamicArray* inputs, DynamicArray* ouptuts, Operation *op, SDL_Point *spawn_pos, const char *name);
Node *create_group_node(SDL_Point *spawn_pos, DynamicArray* inputs, DynamicArray* ouptuts, const char *name, DynamicArray *sub_nodes, DynamicArray *sub_connections, int is_expanded);
SDL_Rect calculate_outline_rect(DynamicArray *sub_nodes, DynamicArray *sub_connections);
SDL_Point calculate_pos_from_outline_rect(SDL_Rect outline_rect, SDL_Rect node_rect);
SDL_Point *find_most_top_left(DynamicArray *nodes);
SDL_Rect calc_rect(SDL_Point *spawn_pos, int num_inputs, int num_outputs, const char *name);
void free_node(Node *node);

#endif // NODE_H
