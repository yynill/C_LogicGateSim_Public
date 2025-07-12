#ifndef CONNECTION_H
#define CONNECTION_H
#pragma once


#include <SDL2/SDL.h>
#include <assert.h>
#include "DataStructures/DynamicArray.h"
#include "DataStructures/HashMap.h"
#include "node.h"
#include "renderer.h"

typedef struct Pin Pin;
typedef struct Connection Connection;

typedef struct Connection_point
{
    int x;
    int y;
    DynamicArray *neighbors; // struct Connection_points
    Connection *parent_connection;
    Pin *linked_to_pin;
} Connection_point; // Undirected, cyclic allowed, Graph

typedef struct Connection {
    DynamicArray *input_pins;  // struct Pin
    DynamicArray *output_pins; // struct Pin
    DynamicArray *points;      // struct Connection_point
    int state;
} Connection;

Connection *start_connection(Pin *pin);
Connection_point *create_connection_point(Connection *con, int x, int y, Pin *linked_pin);
Connection_point * add_connection_point(Connection *con, int x, int y, Pin *linked_pin);
void free_connection_point(Connection_point *point);
void add_connection_link(SimulationState *state, Connection_point *from, Connection_point *to);
void remove_connection_link(Connection_point *from, Connection_point *to);
void add_pin(Connection *con, Pin *pin);
void finalize_connection(Connection *con, Pin *pin);
void roll_back_connection_branch(Connection *con, Connection_point *first_point, Connection_point *last_point);
void delete_connection_branch(SimulationState *state, Connection_point *point, Pin* pin_of_point);
Connection_point *find_connection_point_with_pin(Connection *con, Pin *pin);
void propagate_state(Connection *con);
void update_connection_geometry(Connection *con);
void unmerge_connection(SimulationState *state, Connection *con, Connection_point *point1, Connection_point *point2);

DynamicArray* find_fully_selected_connections(DynamicArray *selected_connection_points);
Connection* copy_connection(SimulationState *state, Connection *original_conn, DynamicArray *source_nodes, DynamicArray *target_nodes, float offset_x, float offset_y);

void print_connection(Connection *con);
void print_connection_graph(Connection *con);
void free_connection(Connection *con);

#endif // CONNECTION_H
