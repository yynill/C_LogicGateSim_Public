#ifndef SIMULATION_H
#define SIMULATION_H
#pragma once

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <dirent.h>

#include "main.h"
#include "renderer.h"
#include "DataStructures/DynamicArray.h"
#include "node.h"
#include "point.h"
#include "button.h"
#include "connection.h"
#include "popup_state.h"
#include "file_handler.h"

typedef struct Node Node;
typedef struct Connection Connection;
typedef struct Connection_point Connection_point;
typedef struct Button Button;
typedef struct PopupState PopupState;

typedef struct SimulationState {
    int is_running;
    int should_reset;

    PopupState *popup_state;

    DynamicArray *nodes;
    DynamicArray *connections;
    DynamicArray *buttons;

    Uint32 last_knife_record_time;
    DynamicArray *knife_stroke; // SDL_Point

    DynamicArray *selected_nodes;
    DynamicArray *selected_connection_points;

    DynamicArray *clipboard_nodes;

    int left_mouse_down;
    int right_mouse_down;
    int middle_mouse_down;

    int mouse_x;
    int mouse_y;
    int mouse_wheel;

    // relative position between where you clicked and the objects origin
    int drag_offset_x;
    int drag_offset_y;

    SDL_Rect selection_box;

    int is_node_dragging;
    int is_camera_dragging;
    int is_cable_dragging;
    int is_connection_point_dragging;
    int is_knife_dragging;
    int is_selection_box_drawing;

    int camera_x;
    int camera_y;
    float camera_zoom;

    int is_paused;

    Node *dragged_node;
    Node *last_dragged_node;
    int last_node_x;
    int last_node_y;

    Connection_point *hovered_connection_point;
    Connection_point *dragging_connection_point;
    Connection_point *first_connection_point;
    Connection_point *last_connection_point;

    Connection *new_connection;

    Pin *hovered_pin;
} SimulationState;

// Function declarations
SimulationState *simulation_init();
void simulation_cleanup();
void simulation_update();
void reset_sim(void *function_data);

void process_left_click();
void process_left_mouse_up();
void process_right_mouse_up();
void process_right_click();
void process_mouse_motion();

void handle_copy();
void handle_paste();
void handle_backspace();
void handle_g_pressed();
void handle_s_pressed();
void handle_l_pressed();
void handle_r_pressed();
void handle_escape(void *function_data);
void handle_enter(void *function_data);

void null_function(void *function_data);
void null_function_wo_data();
void add_node(void *function_data);
void cut_connection();
void one_step(void *function_data);
void toggle_play_pause(void *function_data);
void start_selection_box();
void update_all_connections(DynamicArray *connections, DynamicArray *nodes);
void rename_note();
void delete_selected();
void delete_node_and_connections(Node *node);
Button *find_button_at_position(DynamicArray *buttons, int screen_x, int screen_y);
Node *find_node_at_position(DynamicArray *nodes, float x, float y);
void start_selection_box();
int try_draw_selection_box();
int try_handle_node_dragging(float world_x, float world_y);
int try_hover_connection_point(float world_x, float world_y);
int try_drag_connection_point(float world_x, float world_y);
int try_update_pin_hover(float world_x, float world_y);
int try_handle_knife_stroke_motion(float world_x, float world_y);
int try_handle_node_left_click();
int try_handle_connection_point_left_click();
int try_handle_connection_point_right_click();
int try_handle_connection_right_click();
int try_add_connection_point();
int try_handle_button_click();
int try_handle_pin_click();
int try_complete_connection();

#endif // SIMULATION_H
