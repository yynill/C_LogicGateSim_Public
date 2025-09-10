#ifndef NODE_H
#define NODE_H
#pragma once

#include "main.h"
#include "coordinate_system.h"
#include "operation.h"

typedef struct DynamicArray DynamicArray;
typedef struct Button Button;
typedef struct SDL_Point SDL_Point;
typedef struct Pin Pin;

typedef struct Node {
    char *name;
    Float_Rect rect;

    DynamicArray *inputs;  // pins
    DynamicArray *outputs; // pins
    Operation *operation;

    DynamicArray *sub_nodes;
    DynamicArray *sub_connections;
    struct Node *parent;

    DynamicArray *input_mappings; // PinMapping
    DynamicArray *output_mappings; // PinMapping
} Node;

typedef struct PinMapping {
    Pin *outer_pin;
    Pin *inner_pin;
} PinMapping;

// functions
void print_node(Node *node);
void run_node(Node *node);
void move_node(Node *node, float dx, float dy);

Node *create_node(int num_inputs, int num_outputs, Operation *op, SDL_Point *spawn_pos, const char *name);
Node *create_group_node(SDL_Point *spawn_pos, int num_inputs, int num_outputs, const char *name, DynamicArray *sub_nodes, DynamicArray *sub_connections);
Node *copy_node(Node *node);
void reposition_node_pins(Node *node);

Float_Rect calc_rect(SDL_Point *spawn_pos, int num_inputs, int num_outputs, const char *name, Operation *operation);
SDL_Point calculate_pos_for_group_node(DynamicArray *sub_nodes, DynamicArray *sub_connections);
void free_node(Node *node);

void create_pin_mappings(Node *group_node);
PinMapping *create_pin_mapping(Pin *outer_pin, Pin *inner_pin);
PinMapping *find_pin_mapping(Node *node, Pin *pin);
void add_pin_mapping(Node *added_node);
void remove_pin_mapping(Node *node);

#endif // NODE_H
