#ifndef NODE_H
#define NODE_H
#pragma once

#include "main.h"
typedef struct DynamicArray DynamicArray;
typedef struct Button Button;
typedef struct SDL_Point SDL_Point;

typedef struct Node {
    char *name;
    int is_expanded;
    Float_Rect rect;

    DynamicArray *inputs;  // pins
    DynamicArray *outputs; // pins
    Operation *operation;

    DynamicArray *sub_nodes;
    DynamicArray *sub_connections;
    struct Node *parent;
    Float_Rect outline_rect;
    Button *close_btn;
} Node;

// functions
void print_node(Node *node);
void run_node(Node *node);
void move_group_node_pins(Node *node);
void reshape_outline_box(Node *node);

void handle_close_group(void *function_data);
void move_node(Node *node, float dx, float dy);

Node *create_node(int num_inputs, int num_outputs, Operation *op, SDL_Point *spawn_pos, const char *name);
Node *create_group_node(SDL_Point *spawn_pos, int num_inputs, int num_outputs, const char *name, DynamicArray *sub_nodes, DynamicArray *sub_connections, int is_expanded);

Node *copy_node(Node *node);

Float_Rect calculate_outline_rect(DynamicArray *sub_nodes, DynamicArray *sub_connections);
SDL_Point calculate_pos_from_outline_rect(Float_Rect outline_rect, Float_Rect node_rect);
SDL_Point *find_most_top_left(DynamicArray *nodes);
Float_Rect calc_rect(SDL_Point *spawn_pos, int num_inputs, int num_outputs, const char *name);
void free_node(Node *node);

#endif // NODE_H
