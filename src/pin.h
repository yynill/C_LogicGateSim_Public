
#ifndef PIN_H
#define PIN_H
#pragma once

#include <assert.h>
#include "node.h"


typedef struct Node Node;
typedef struct SimulationState SimulationState;
typedef struct Connection Connection;
typedef struct Connection_point Connection_point;

typedef struct Pin {
    int x, y; // rel pos to node
    int is_input;
    Node *parent_node;
    DynamicArray *connected_connections;
    int state;
    int id;
} Pin;

Pin *create_pin(int x, int y, int ii, Node *parent_node, int id);
Pin* find_corresponding_pin(Pin *original_pin, DynamicArray *original_nodes, DynamicArray *pasted_nodes);
Pin *find_pin_by_id(DynamicArray *nodes, int id);
void print_pin(Pin *p);

extern int next_pin_id;

#endif // PIN_H
