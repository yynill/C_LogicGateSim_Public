
#ifndef PIN_H
#define PIN_H
#pragma once
#include "main.h"
// Forward declarations
typedef struct Node Node;
typedef struct DynamicArray DynamicArray;

typedef struct Pin {
    float x, y; // rel pos to node
    int is_input;
    Node *parent_node;
    DynamicArray *connected_connections;
    int state;
    int id;
} Pin;

Pin *create_pin(int ii, Node *parent_node);
Pin* find_corresponding_pin(Pin *original_pin, DynamicArray *original_nodes, DynamicArray *pasted_nodes);
Pin *find_pin_by_id(DynamicArray *nodes, int id);
void print_pin(Pin *p);

extern int next_pin_id;

#endif // PIN_H
