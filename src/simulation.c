#include "simulation.h"

SimulationState *simulation_init() {
    SimulationState *state = malloc(sizeof(SimulationState));
    if (!state) return NULL;

    state->is_running = 1;

    state->popup_state = NULL;

    state->nodes = array_create(16);
    assert(state->nodes != NULL);

    state->connections = array_create(16);
    assert(state->connections != NULL);

    state->buttons = array_create(16);
    assert(state->buttons != NULL);

    state->knife_stroke = array_create(16);
    assert(state->knife_stroke != NULL);

    state->selected_nodes = array_create(16);
    assert(state->selected_nodes != NULL);

    state->selected_connection_points = array_create(16);
    assert(state->selected_connection_points != NULL);

    array_add(state->buttons, create_button((Float_Rect){10, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "NOT", notGate, add_node));
    array_add(state->buttons, create_button((Float_Rect){80, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "AND", andGate, add_node));
    array_add(state->buttons, create_button((Float_Rect){150, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "OR", orGate, add_node));
    array_add(state->buttons, create_button((Float_Rect){220, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "XOR", xorGate, add_node));
    array_add(state->buttons, create_button((Float_Rect){290, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "XNOR", xnorGate, add_node));
    array_add(state->buttons, create_button((Float_Rect){360, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "NOR", norGate, add_node));
    array_add(state->buttons, create_button((Float_Rect){430, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "NAND", nandGate, add_node));
    array_add(state->buttons, create_button((Float_Rect){550, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "SWITCH", switchNode, add_node));
    array_add(state->buttons, create_button((Float_Rect){620, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "LIGHT", lightNode, add_node));
    array_add(state->buttons, create_button((Float_Rect){690, 10, BUTTON_WIDTH, BUTTON_HEIGHT}, "NOTE", noteNode, add_node));

    array_add(state->buttons, create_button((Float_Rect){WINDOW_WIDTH - 4 * (BUTTON_HEIGHT + 10), 15, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2}, "/assets/images/step_back.png", nullGate, null_function));
    array_add(state->buttons, create_button((Float_Rect){WINDOW_WIDTH - 3 * (BUTTON_HEIGHT + 10), 15, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2}, "/assets/images/play.png", nullGate, toggle_play_pause));
    array_add(state->buttons, create_button((Float_Rect){WINDOW_WIDTH - 2 * (BUTTON_HEIGHT + 10), 15, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2}, "/assets/images/step_forth.png", nullGate, one_step));
    array_add(state->buttons, create_button((Float_Rect){WINDOW_WIDTH - 1 * (BUTTON_HEIGHT + 10), 15, BUTTON_HEIGHT / 2, BUTTON_HEIGHT / 2}, "/assets/images/reload.png", nullGate, reset_sim));

    state->left_mouse_down = 0;
    state->right_mouse_down = 0;
    state->middle_mouse_down = 0;

    state->mouse_x = 0;
    state->mouse_y = 0;
    state->mouse_wheel = 0;
    state->drag_offset_x = 0;
    state->drag_offset_y = 0;

    state->is_selection_box_drawing = 0;
    state->selection_box = (Float_Rect){0, 0, 0, 0};

    state->is_node_dragging = 0;
    state->is_camera_dragging = 0;
    state->is_cable_dragging = 0;
    state->is_connection_point_dragging = 0;
    state->is_knife_dragging = 0;

    state->camera_x = 0;
    state->camera_y = 0;
    state->camera_zoom = 1;

    state->is_paused = 1;
    state->node_queue = init_queue();
    state->step_count = 0;

    state->hovered_connection_point = NULL;
    state->dragging_connection_point = NULL;
    state->first_connection_point = NULL;
    state->last_connection_point = NULL;
    state->new_connection = NULL;

    state->dragged_node = NULL;

    state->last_node_x = 0;
    state->last_node_y = 0;

    state->hovered_pin = NULL;

    state->clipboard_nodes = NULL;

    state->subnode_window_parent = NULL;
    state->subnode_window_button = create_button((Float_Rect){10, TOP_BAR_HEIGHT + 10, 100, 30}, "Leave Group", NULL, leave_subnode_view);

    return state;
}

void simulation_cleanup() {
    if (sim_state) {
        for (int i = 0; i < sim_state->nodes->size; i++) {
            delete_node_and_connections((Node*)array_get(sim_state->nodes, i));
        }
        free(sim_state->nodes);

        for (int i = 0; i < sim_state->connections->size; i++) {
            free_connection((Connection*)array_get(sim_state->connections, i));
        }
        free(sim_state->connections);

        array_free(sim_state->buttons);
        array_free(sim_state->knife_stroke);
        free_queue(sim_state->node_queue);
        free(sim_state);
    }
}

void reset_sim(void *function_data) {
    (void)function_data;
    sim_state->should_reset = 1;
}

void simulation_update() {

    if (!sim_state->is_paused) {
        one_step("");
    }

    // for (int i = 0; i < sim_state->nodes->size; i++) {
    //     Node *node = array_get(sim_state->nodes, i);
    //     printf("🔵loop\n");
    //     print_node(node);

    //     if (node->sub_nodes != NULL) {
    //         for (int j = 0; j < node->sub_nodes->size; j++) {
    //             Node *sub_node = array_get(node->sub_nodes, j);
    //             printf("🟠sub_node\n");
    //             print_node(sub_node);
    //         }
    //     }

    //     if (node->sub_connections != NULL) {
    //         for (int j = 0; j < node->sub_connections->size; j++) {
    //             Connection *sub_con = array_get(node->sub_connections, j);
    //             printf("🟢sub_con\n");
    //             print_connection(sub_con);
    //         }
    //     }
    // }

    // for (int i = 0; i < sim_state->connections->size; i++) {
    //     Connection *con = array_get(sim_state->connections, i);
    //     printf("🔵loop\n");
    //     print_connection(con);
    // }
}

void null_function(void *function_data) {
    (void)function_data;
    printf("// todo\n");
    return;
}

void null_function_wo_data() {
    printf("// todo\n");
    return;
}

void add_node(void *function_data) {
    assert(function_data != NULL);
    assert(sim_state->nodes != NULL);

    Button *button = (Button *)function_data;
    Operation *op = (Operation *)button->function_data;

    float world_x, world_y;
    screen_point_to_world(100, 100, &world_x, &world_y);
    SDL_Point node_pos = {.x = world_x, .y = world_y};

    int num_inputs = 2;
    int num_outputs = 1;
    if (op == notGate) {
        num_inputs = 1;
    }
    if (op == switchNode) {
        num_inputs = 0;
        num_outputs = 1;
    }
    if (op == lightNode) {
        num_inputs = 1;
        num_outputs = 0;
    }
    if (op == noteNode) {
        num_inputs = 0;
        num_outputs = 0;
    }

    DynamicArray *current_nodes = get_current_node_layer();
    Node *new_node = create_node(num_inputs, num_outputs, op, &node_pos, button->name);
    array_add(current_nodes, new_node);

    if (sim_state->subnode_window_parent != NULL) {
        new_node->parent = sim_state->subnode_window_parent;
    }

    add_pin_mapping(new_node);
    if (!queue_contains(sim_state->node_queue, new_node)) {
        enqueue(sim_state->node_queue, new_node);
    }
}

void cut_connection() {
    assert(sim_state->connections != NULL);
    assert(sim_state->knife_stroke != NULL);

    if (sim_state->knife_stroke->size < 2) {
        goto end;
    }

    DynamicArray *current_connections = get_current_connection_layer();
    DynamicArray *connections_to_delete = array_create(4);
    DynamicArray *corresponding_points_to_delete = array_create(4);

    for (int l = 0; l < sim_state->knife_stroke->size - 1; l++) {
        SDL_Point *knife_p1 = array_get(sim_state->knife_stroke, l);
        SDL_Point *knife_p2 = array_get(sim_state->knife_stroke, l + 1);
        
        for (int i = 0; i < current_connections->size; i++) {
            Connection *con = array_get(current_connections, i);
            if (con == NULL || con->points == NULL || con->points->size < 2) continue;

            for (int j = 0; j < con->points->size; j++) {
                Connection_point *con_p1 = array_get(con->points, j);

                for (int k = 0; k < con_p1->neighbors->size; k++) {
                    Connection_point *con_p2 = array_get(con_p1->neighbors, k);
                    if (con_p1 < con_p2) continue;

                    SDL_Point intersection;
                    if (segment_intersection(knife_p1, knife_p2, &(SDL_Point){con_p1->pos.x, con_p1->pos.y}, &(SDL_Point){con_p2->pos.x, con_p2->pos.y}, &intersection)) {
                        array_add(connections_to_delete, con);
                        array_add(corresponding_points_to_delete, con_p1);
                        break;
                    }
                }
            }
        }
    }

    for (int i = 0; i < connections_to_delete->size; i++) {
        Connection *con_to_delete = array_get(connections_to_delete, i);
        Connection_point *start_point = array_get(corresponding_points_to_delete, i);
        if (con_to_delete->points->size > 0) {
            delete_connection_branch(start_point, current_connections);
        }
    }

    free(connections_to_delete);
    free(corresponding_points_to_delete);
end:
    array_free(sim_state->knife_stroke);
    sim_state->knife_stroke = array_create(16);
    sim_state->is_knife_dragging = 0;
}

void toggle_play_pause(void *function_data) {
    (void)function_data;

    sim_state->is_paused = !sim_state->is_paused;
}

void propagate_all_connections(DynamicArray *connections) {
    if (!connections) return;

    for (int i = 0; i < connections->size; i++) {
        Connection *con = array_get(connections, i);
        propagate_state(con);
    }
}

void propagate_all_connections_recursive(DynamicArray *connections) {
    if (!connections) return;

    for (int i = 0; i < connections->size; i++) {
        Connection *con = array_get(connections, i);
        propagate_state(con);
    }
}

void propagate_all_connections_in_nodes(DynamicArray *nodes) {
    if (!nodes) return;

    for (int i = 0; i < nodes->size; i++) {
        Node *node = array_get(nodes, i);
        if (node->sub_connections) {
            propagate_all_connections_recursive(node->sub_connections);
        }
        if (node->sub_nodes) {
            propagate_all_connections_in_nodes(node->sub_nodes);
        }
    }
}

void one_step(void *function_data) {
    (void)function_data;

    propagate_all_connections(sim_state->connections);
    propagate_all_connections_in_nodes(sim_state->nodes);

    Node *node = dequeue(sim_state->node_queue);
    if (node == NULL) return;

    run_node(node);

    print_queue(sim_state->node_queue);
    sim_state->step_count++;
}

int try_handle_node_left_click() {
    float world_x, world_y;
    screen_point_to_world(sim_state->mouse_x, sim_state->mouse_y, &world_x, &world_y);

    Node *node = find_node_at_position(world_x, world_y);
    if (node == NULL) return 0;

    sim_state->drag_offset_x = world_x - node->rect.x;
    sim_state->drag_offset_y = world_y - node->rect.y;
    sim_state->is_node_dragging = 1;
    sim_state->dragged_node = node;

    return 1;
}

int try_handle_connection_point_left_click() {
    if (sim_state->hovered_connection_point == NULL) return 0;

    sim_state->new_connection = sim_state->hovered_connection_point->parent_connection;
    sim_state->first_connection_point = sim_state->hovered_connection_point;
    sim_state->last_connection_point = sim_state->hovered_connection_point;
    sim_state->is_cable_dragging = 1;

    return 1;
}

int try_handle_connection_point_right_click() {
    if (sim_state->hovered_connection_point == NULL) return 0;

    float world_x, world_y;
    screen_point_to_world(sim_state->mouse_x, sim_state->mouse_y, &world_x, &world_y);

    sim_state->is_connection_point_dragging = 1;
    sim_state->dragging_connection_point = sim_state->hovered_connection_point;
    sim_state->drag_offset_x = world_x - sim_state->hovered_connection_point->pos.x;
    sim_state->drag_offset_y = world_y - sim_state->hovered_connection_point->pos.y;
    return 1;
}

int try_handle_connection_right_click() {
    float world_x, world_y;
    screen_point_to_world(sim_state->mouse_x, sim_state->mouse_y, &world_x, &world_y);

    Connection *closest_connection = NULL;
    Connection_point *closest_p1 = NULL;
    Connection_point *closest_p2 = NULL;
    float closest_distance = 10.0f;

    DynamicArray *current_connections = get_current_connection_layer();
    for (int i = 0; i < current_connections->size; i++) {
        Connection *con = array_get(current_connections, i);

        for (int j = 0; j < con->points->size; j++) {
            Connection_point *p1 = array_get(con->points, j);

            for (int k = 0; k < p1->neighbors->size; k++) {
                Connection_point *p2 = array_get(p1->neighbors, k);

                if (p1 < p2) {
                    float distance = distance_to_line_segment(world_x, world_y, p1->pos.x, p1->pos.y, p2->pos.x, p2->pos.y);

                    if (distance < closest_distance) {
                        closest_distance = distance;
                        closest_connection = con;
                        closest_p1 = p1;
                        closest_p2 = p2;
                    }
                }
            }
        }
    }

    if (closest_connection != NULL && closest_p1 != NULL && closest_p2 != NULL) {
        Connection_point *new_point = add_connection_point(closest_connection, (int)world_x, (int)world_y, NULL);

        array_remove(closest_p1->neighbors, closest_p2);
        array_remove(closest_p2->neighbors, closest_p1);

        add_connection_link(closest_p1, new_point);
        add_connection_link(new_point, closest_p2);

        update_connection_geometry(closest_connection);

        return 1;
    }

    return 0;
}

int try_handle_button_click() {
    Button *clicked_button = find_button_at_position(sim_state->buttons, sim_state->mouse_x, sim_state->mouse_y);

    if (clicked_button) {
        clicked_button->on_press(clicked_button);
        return 1;
    }

    if (sim_state->subnode_window_parent != NULL) {
        if (sim_state->subnode_window_button != NULL) {
            if (sim_state->mouse_x >= sim_state->subnode_window_button->rect.x &&
                sim_state->mouse_x <= sim_state->subnode_window_button->rect.x + sim_state->subnode_window_button->rect.w &&
                sim_state->mouse_y >= sim_state->subnode_window_button->rect.y &&
                sim_state->mouse_y <= sim_state->subnode_window_button->rect.y + sim_state->subnode_window_button->rect.h) {
                sim_state->subnode_window_button->on_press(sim_state->subnode_window_button);
                return 1;
            }
        }
    }

    return 0;
}

int try_handle_pin_click() {
    if (sim_state->hovered_pin != NULL) {
        sim_state->new_connection = start_connection(sim_state->hovered_pin);

        float world_mouse_x, world_mouse_y;
        screen_point_to_world(sim_state->mouse_x, sim_state->mouse_y, &world_mouse_x, &world_mouse_y);

        Connection_point *point = add_connection_point(sim_state->new_connection, (int)world_mouse_x, (int)world_mouse_y, sim_state->hovered_pin);
        sim_state->first_connection_point = point;
        sim_state->last_connection_point = point;
        sim_state->is_cable_dragging = 1;
        return 1;
    }
    return 0;
}

void start_selection_box() {
    sim_state->selected_nodes->size = 0;
    sim_state->selected_connection_points->size = 0;

    sim_state->is_selection_box_drawing = 1;
    sim_state->selection_box.x = sim_state->mouse_x;
    sim_state->selection_box.y = sim_state->mouse_y;
}

void update_all_connections(DynamicArray *connections, DynamicArray *nodes) {
    for (int i = 0; i < connections->size; i++) {
        Connection *con = array_get(connections, i);
        update_connection_geometry(con);
    }

    for (int i = 0; i < nodes->size; i++) {
        Node *node = array_get(nodes, i);
        if (node->sub_nodes != NULL) {
            update_all_connections(node->sub_connections, node->sub_nodes);
        }
    }
}

int try_draw_selection_box() {
    if (!sim_state->is_selection_box_drawing) return 0;

    sim_state->selection_box.w = sim_state->mouse_x - sim_state->selection_box.x;
    sim_state->selection_box.h = sim_state->mouse_y - sim_state->selection_box.y;
    return 1;
}

int try_handle_node_dragging(float world_x, float world_y) {
    if (!sim_state->is_node_dragging || sim_state->dragged_node == NULL) return 0;

    float offset_x = world_x - sim_state->drag_offset_x - sim_state->dragged_node->rect.x;
    float offset_y = world_y - sim_state->drag_offset_y - sim_state->dragged_node->rect.y;

    move_node(sim_state->dragged_node, offset_x, offset_y);

    for (int i = 0; i < sim_state->selected_nodes->size; i++) {
        Node *current = array_get(sim_state->selected_nodes, i);
        if (current != sim_state->dragged_node) {
            move_node(current, offset_x, offset_y);
        }
    }

    for (int i = 0; i < sim_state->selected_connection_points->size; i++) {
        Connection_point *pt = array_get(sim_state->selected_connection_points, i);
        pt->pos.x += offset_x;
        pt->pos.y += offset_y;
    }

    update_all_connections(sim_state->connections, sim_state->nodes);
    return 1;
}

DynamicArray* get_all_connection_points() {
    DynamicArray *result = array_create(16);
    DynamicArray *current_connections = get_current_connection_layer();

    for (int i = 0; i < current_connections->size; i++) {
        Connection *con = array_get(current_connections, i);
        for (int j = 0; j < con->points->size; j++) {
            Connection_point *point = array_get(con->points, j);
            array_add(result, point);
        }
    }

    return result;
}

int try_handle_connection_point_dragging(float world_x, float world_y) {
    if (!sim_state->is_connection_point_dragging || !sim_state->dragging_connection_point) return 0;

    sim_state->hovered_connection_point = sim_state->dragging_connection_point;

    sim_state->dragging_connection_point->pos.x = world_x - sim_state->drag_offset_x - CONNECTION_POINT_SIZE;
    sim_state->dragging_connection_point->pos.y = world_y - sim_state->drag_offset_y - CONNECTION_POINT_SIZE;
    update_connection_geometry(sim_state->dragging_connection_point->parent_connection);

    return 1;
}

int try_hover_connection_point(float world_x, float world_y) {
    DynamicArray *connection_points = get_all_connection_points();

    for (int i = 0; i < connection_points->size; i++) {
        Connection_point *point = array_get(connection_points, i);

        int dx = world_x - point->pos.x;
        int dy = world_y - point->pos.y;
        float distance_squared = (dx * dx) + (dy * dy);
        float hover_radius_squared = 25.0f;

        if (distance_squared <= hover_radius_squared) {
            sim_state->hovered_connection_point = point;
            return 1;
        }
        else {
            sim_state->hovered_connection_point = NULL;
        }
    }
    return 0;
}


int try_update_pin_hover(float world_x, float world_y) {
    sim_state->hovered_pin = NULL;
    DynamicArray *current_nodes = get_current_node_layer();
    for (int i = 0; i < current_nodes->size; i++) {
        Node *node = array_get(current_nodes, i);

        for (int n = 0; n < node->inputs->size; n++) {
            Pin *pin = (Pin *)array_get(node->inputs, n);

            if (world_x >= pin->x + node->rect.x && world_x <= pin->x + node->rect.x + PIN_SIZE &&
                world_y >= pin->y + node->rect.y && world_y <= pin->y + node->rect.y + PIN_SIZE) {
                sim_state->hovered_pin = pin;
                return 1;
            }
        }

        for (int n = 0; n < node->outputs->size; n++) {
            Pin *pin = (Pin *)array_get(node->outputs, n);
            if (world_x >= pin->x + node->rect.x && world_x <= pin->x + node->rect.x + PIN_SIZE &&
                world_y >= pin->y + node->rect.y && world_y <= pin->y + node->rect.y + PIN_SIZE) {
                sim_state->hovered_pin = pin;
                return 1;
            }
        }
    }
    return 0;
}

int try_handle_knife_stroke_motion(float world_x, float world_y) {
    if (!sim_state->is_knife_dragging) return 0;
    if (!sim_state->right_mouse_down) return 0;

    Uint32 now = SDL_GetTicks();
    if (now - sim_state->last_knife_record_time > 20) {
        SDL_Point *point = malloc(sizeof(SDL_Point));
        if (!point) {
            printf("Out of memory!\n");
            exit(EXIT_FAILURE);
        }
        point->x = world_x;
        point->y = world_y;
        array_add(sim_state->knife_stroke, point);
        sim_state->last_knife_record_time = now;
    }
    return 1;
}

int point_in_rect(float x, float y, Float_Rect rect) {
    return x >= rect.x && x <= rect.x + rect.w &&
           y >= rect.y && y <= rect.y + rect.h;
}

int point_in_node(float world_x, float world_y, Node *node) {
    return point_in_rect(world_x, world_y, node->rect);
}

DynamicArray *get_current_connection_layer() {
    if (sim_state->subnode_window_parent != NULL && sim_state->subnode_window_parent->sub_connections != NULL) {
        return sim_state->subnode_window_parent->sub_connections;
    }
    return sim_state->connections;
}

DynamicArray *get_connection_layer_of_node(Node *node) {
    if (node->parent != NULL) {
        return node->parent->sub_connections;
    }
    else {
        return sim_state->connections;
    }
}

DynamicArray *get_current_node_layer() {
    if (sim_state->subnode_window_parent != NULL && sim_state->subnode_window_parent->sub_nodes != NULL) {
        return sim_state->subnode_window_parent->sub_nodes;
    }
    return sim_state->nodes;
}

Node *find_node_at_position(float x, float y) {
    DynamicArray *current_nodes = get_current_node_layer();
    for (int i = current_nodes->size - 1; i >= 0 ; i--) {
        Node *node = array_get(current_nodes, i);
        if (!point_in_node(x, y, node)) continue;

        return node;
    }
    return NULL;
}

Button *find_button_at_position(DynamicArray *buttons, int screen_x, int screen_y) {
    assert(buttons != NULL);

    for (int i = 0; i < buttons->size; i++) {
        Button *button = array_get(buttons, i);
        if (point_in_rect(screen_x, screen_y, button->rect)) {
            return button;
        }
    }
    return NULL;
}

void toggle_switch_outputs(Node *switch_node) {
    for (int i = 0; i < switch_node->outputs->size; i++) {
        Pin *pin = array_get(switch_node->outputs, i);
        pin->state = !pin->state;
    }
    if (!queue_contains(sim_state->node_queue, switch_node)) {
        enqueue(sim_state->node_queue, switch_node);
    }
}

void clean_up_connection() {
    if (sim_state->new_connection == NULL) return;
    if (!sim_state->is_cable_dragging) return;

    roll_back_connection_branch(sim_state->new_connection, sim_state->first_connection_point, sim_state->last_connection_point);

    sim_state->new_connection = NULL;
    sim_state->first_connection_point = NULL;
    sim_state->last_connection_point = NULL;
    sim_state->is_connection_point_dragging = 0;
    sim_state->is_cable_dragging = 0;
}

int try_complete_connection() {
    float world_mouse_x, world_mouse_y;
    screen_point_to_world(sim_state->mouse_x, sim_state->mouse_y, &world_mouse_x, &world_mouse_y);

    if (sim_state->hovered_connection_point != NULL) {

        add_connection_link(sim_state->hovered_connection_point, sim_state->last_connection_point);

        sim_state->first_connection_point = NULL;
        sim_state->last_connection_point = NULL;
        sim_state->is_cable_dragging = 0;
        return 1;
    }
    else if (sim_state->hovered_pin != NULL) {
        Connection_point *point = add_connection_point(sim_state->new_connection, (int)world_mouse_x, (int)world_mouse_y, sim_state->hovered_pin);
        add_connection_link(point, sim_state->last_connection_point);
        finalize_connection(sim_state->new_connection, sim_state->hovered_pin);

        DynamicArray *current_connections = get_current_connection_layer();
        if (!array_contains(current_connections, sim_state->new_connection)) {
            array_add(current_connections, sim_state->new_connection);
        }

        sim_state->first_connection_point = NULL;
        sim_state->last_connection_point = NULL;
        sim_state->is_cable_dragging = 0;
        return 1;
    }
    else {
        clean_up_connection();
        return 0;
    }
}

void delete_node_and_connections(Node *node) {
    if (!node) return;

    queue_find_and_remove(sim_state->node_queue, node);

    if (node->sub_nodes != NULL) {
        for (int i = 0; i < node->sub_nodes->size; i ++) {
            Node *sub_node = array_get(node->sub_nodes, i);
            delete_node_and_connections(sub_node);
        }
        node->sub_nodes = NULL;
    }

    if (node->sub_connections != NULL) {
        for (int i = 0; i < node->sub_connections->size; i++) {
            Connection *sub_con = array_get(node->sub_connections, i);
            free_connection(sub_con);
        }
        node->sub_connections->size = 0;
    }

    for (int i = 0; i < node->inputs->size; i++) {
        Pin *i_pin = array_get(node->inputs, i);
        DynamicArray *connections_copy = flat_copy(i_pin->connected_connections);
        for (int j = 0; j < connections_copy->size; j++) {
            Connection *con = array_get(connections_copy, j);
            Connection_point *point = find_connection_point_with_pin(con, i_pin);
            DynamicArray *connection_layer = get_connection_layer_of_node(node);
            delete_connection_branch(point, connection_layer);
        }
        free(connections_copy);
    }

    for (int i = 0; i < node->outputs->size; i++) {
        Pin *o_pin = array_get(node->outputs, i);
        DynamicArray *connections_copy = flat_copy(o_pin->connected_connections);
        for (int j = 0; j < connections_copy->size; j++) {
            Connection *con = array_get(connections_copy, j);
            Connection_point *point = find_connection_point_with_pin(con, o_pin);
            DynamicArray *connection_layer = get_connection_layer_of_node(node);
            delete_connection_branch(point, connection_layer);
        }
        free(connections_copy);
    }

    if (node->parent != NULL) {
        if (array_contains(node->parent->sub_nodes, node)) {
            remove_pin_mapping(node);
            array_remove(node->parent->sub_nodes, node);
        }
    }
    else {
        DynamicArray *current_nodes = get_current_node_layer();
        if (array_contains(current_nodes, node)) {
            array_remove(current_nodes, node);
        }
    }

    free_node(node);
}

int try_handle_selection() {
    assert(sim_state->nodes != NULL);

    if (sim_state->is_selection_box_drawing == 0) return 0;

    Float_Rect selection_box_world;
    screen_rect_to_world(&sim_state->selection_box, &selection_box_world);

    DynamicArray *current_nodes = get_current_node_layer();
    DynamicArray *current_connections = get_current_connection_layer();

    for (int i = 0; i < current_nodes->size; i++) {
        Node *node = array_get(current_nodes, i);
        SDL_Rect selection_box_rect = float_rect_to_sdl_rect(&selection_box_world);
        SDL_Rect node_rect = float_rect_to_sdl_rect(&node->rect);
        if (SDL_HasIntersection(&selection_box_rect, &node_rect)) {
            array_add(sim_state->selected_nodes, node);
        }
    }

    for (int i = 0; i < current_connections->size; i++) {
        Connection *con = array_get(current_connections, i);
        for (int j = 0; j < con->points->size; j++) {
            Connection_point *current = array_get(con->points, j);
            if (point_in_rect(current->pos.x, current->pos.y, selection_box_world)) {
                array_add(sim_state->selected_connection_points, current);
            }
        }
    }

    sim_state->is_selection_box_drawing = 0;
    sim_state->selection_box = (Float_Rect){0, 0, 0, 0};
    return 1;
}

void rename_node() {
    for (int i = 0; i < sim_state->selected_nodes->size; i++) {
        Node *note_to_rename = array_get(sim_state->selected_nodes, i);
        if (note_to_rename->name != NULL) {
            free(note_to_rename->name);
        }

        note_to_rename->name = strdup(sim_state->popup_state->name_input.text);
    }

    return;
}

int try_handle_node_right_click() {
    float world_x, world_y;
    screen_point_to_world(sim_state->mouse_x, sim_state->mouse_y, &world_x, &world_y);

    Node *clicked_node = find_node_at_position(world_x, world_y);
    if (clicked_node == NULL) return 0;

    if (clicked_node->operation == switchNode) {
        toggle_switch_outputs(clicked_node);
        return 1;
    }

    if (clicked_node->sub_nodes != NULL) {
        open_subnode_view(clicked_node);
        return 1;
    }

    return 0;
}

void open_subnode_view(Node *clicked_node) {
    sim_state->subnode_window_parent = clicked_node;
    sim_state->selected_connection_points->size = 0;
    sim_state->selected_nodes->size = 0;
    // TODO: adjust camera zoom to fit the subnode positions and back to the original position
}

void leave_subnode_view(void *function_data) {
    (void)function_data;

    if (sim_state->subnode_window_parent->parent != NULL) {
        sim_state->subnode_window_parent = sim_state->subnode_window_parent->parent;
    }
    else {
        sim_state->subnode_window_parent = NULL;
    }
    sim_state->selected_connection_points->size = 0;
    sim_state->selected_nodes->size = 0;
}

int try_add_connection_point() {
    if (!sim_state->is_cable_dragging) return 0;
    if (!sim_state->right_mouse_down) return 0;

    float world_mouse_x, world_mouse_y;
    screen_point_to_world(sim_state->mouse_x, sim_state->mouse_y, &world_mouse_x, &world_mouse_y);

    Connection_point *point = add_connection_point(sim_state->new_connection, (int)world_mouse_x, (int)world_mouse_y, NULL);
    add_connection_link(point, sim_state->last_connection_point);
    sim_state->last_connection_point = point;

    sim_state->right_mouse_down = 0;
    return 1;
}

void process_left_click() {
    assert(sim_state->nodes != NULL);
    assert(sim_state->buttons != NULL);

    if (!sim_state->left_mouse_down) return;

    if (try_handle_button_click()) return;
    if (sim_state->popup_state != NULL) {
        if(try_handle_popup()) return;
    }
    if (try_handle_pin_click()) return;
    if (try_handle_connection_point_left_click()) return;
    if (try_handle_node_left_click()) return;

    start_selection_box();
}

void process_right_click() {
    assert(sim_state->nodes != NULL);

    if (!sim_state->right_mouse_down) return;

    if (try_add_connection_point()) return;
    if (try_handle_node_right_click()) return;
    if (try_handle_connection_point_right_click()) return;
    if (try_handle_connection_right_click()) return;
    else sim_state->is_knife_dragging = 1;
}

void process_mouse_motion() {
    assert(sim_state->nodes != NULL);
    assert(sim_state->buttons != NULL);

    float world_x, world_y;
    screen_point_to_world(sim_state->mouse_x, sim_state->mouse_y, &world_x, &world_y);

    if (try_update_pin_hover(world_x, world_y)) return;
    if (try_hover_connection_point(world_x, world_y)) return;
    if (try_handle_node_dragging(world_x, world_y)) return;
    if (try_handle_connection_point_dragging(world_x, world_y)) return;
    if (try_handle_knife_stroke_motion(world_x, world_y)) return;
    if (try_draw_selection_box()) return;
}

void process_right_mouse_up() {
    sim_state->is_connection_point_dragging = 0;

    if (!sim_state->is_cable_dragging) {
        cut_connection();
    }
}

void process_left_mouse_up() {
    if (try_handle_selection()) return;
    if (try_complete_connection()) return;
}

void handle_copy() {
    sim_state->clipboard_nodes = flat_copy(sim_state->selected_nodes);
}

void handle_paste() {
    if (sim_state->clipboard_nodes == NULL) {
        printf("handle_paste: clipboard is empty, nothing to paste\n");
        return;
    }

    sim_state->selected_nodes->size = 0;
    sim_state->selected_connection_points->size = 0;

    for (int i = 0; i < sim_state->clipboard_nodes->size; i++) {
        Node *current = array_get(sim_state->clipboard_nodes, i);
        SDL_Point pos = {.x = current->rect.x + 10, .y = current->rect.y + 10};
        Node *node_copy;

        if (current->sub_nodes != NULL) {
            node_copy = copy_node(current);
        }
        else {
            node_copy = create_node(current->inputs->size, current->outputs->size, current->operation, &pos, current->name);
        }

        DynamicArray *current_nodes = get_current_node_layer();
        array_add(current_nodes, node_copy);
        array_add(sim_state->selected_nodes, node_copy);

        if (sim_state->subnode_window_parent != NULL) {
            node_copy->parent = sim_state->subnode_window_parent;
            add_pin_mapping(node_copy);
        }

        if (!queue_contains(sim_state->node_queue, node_copy)) {
            enqueue(sim_state->node_queue, node_copy);
        }

        if (node_copy->sub_nodes != NULL) {
            for (int j = 0; j < node_copy->sub_nodes->size; j++) {
                Node *sub_node = array_get(node_copy->sub_nodes, j);
                if (!queue_contains(sim_state->node_queue, sub_node)) {
                    enqueue(sim_state->node_queue, sub_node);
                }
            }
        }
    }

    DynamicArray *matching_connections = find_fully_selected_connections(sim_state->clipboard_nodes);

    for (int i = 0; i < matching_connections->size; i++) {
        Connection *original_conn = array_get(matching_connections, i);
        Connection *new_con = copy_connection(original_conn, sim_state->clipboard_nodes, sim_state->selected_nodes, 10, 10);

        if (new_con != NULL) {
            for (int j = 0; j < new_con->points->size; j++) {
                Connection_point *point = array_get(new_con->points, j);
                array_add(sim_state->selected_connection_points, point);
            }

            DynamicArray *current_connections = get_current_connection_layer();
            array_add(current_connections, new_con);
            propagate_state(new_con);
            update_connection_geometry(new_con);
        }
    }
    free(matching_connections);
}

void handle_backspace() {
    if(sim_state->popup_state != NULL) {
        size_t len = strlen(sim_state->popup_state->name_input.text);
        if (len > 0) {
            sim_state->popup_state->name_input.text[len - 1] = '\0';
        }
        return;
    }
    else {
        delete_selected();
    }
}

void delete_selected() {
    for (int i = 0; i < sim_state->selected_nodes->size; i++) {
        Node *node = array_get(sim_state->selected_nodes, i);
        delete_node_and_connections(node);
    }

    sim_state->last_dragged_node = NULL;
    sim_state->clipboard_nodes = NULL;
    sim_state->selected_nodes->size = 0;
    sim_state->selected_connection_points->size = 0;
}

void handle_group_nodes() {
    int num_inputs = 0;
    int num_outputs = 0;

    for (int i = 0; i < sim_state->selected_nodes->size; i++) {
        Node *node = array_get(sim_state->selected_nodes, i);
        if(node->operation == switchNode)  {
            num_inputs++;
            remove_pin_mapping(node);
        }
        if(node->operation == lightNode)  {
            num_outputs++;
            remove_pin_mapping(node);
        }
    }

    for (int i = 0; i < sim_state->selected_nodes->size; i++) {
        Node *n = array_get(sim_state->selected_nodes, i);
        queue_find_and_remove(sim_state->node_queue, n);
    }

    DynamicArray *matching_connections = find_fully_selected_connections(sim_state->selected_nodes);
    SDL_Point pos = calculate_pos_for_group_node(sim_state->selected_nodes, matching_connections);

    Node *group_node = create_group_node(&pos, num_inputs, num_outputs, sim_state->popup_state->name_input.text, sim_state->selected_nodes, matching_connections);

    sim_state->selected_connection_points->size = 0;
    sim_state->selected_nodes->size = 0;

    DynamicArray *current_nodes = get_current_node_layer();
    array_add(current_nodes, group_node);
    if (!queue_contains(sim_state->node_queue, group_node)) {
        enqueue(sim_state->node_queue, group_node);
    }
    free(matching_connections);
}

void handle_r_pressed() {
    sim_state->popup_state = init_popupstate("Rename Note Node", rename_node);
}

void handle_g_pressed() {
    sim_state->popup_state = init_popupstate("Group Node", handle_group_nodes);
}

void handle_s_pressed() {
    sim_state->popup_state = init_popupstate("Save Selected to JSON", save_graph_to_json);
}

void handle_l_pressed() {
    sim_state->popup_state = init_popupstate("Load JSON", null_function_wo_data);

    DIR *dir = opendir("circuit_files");
    if (dir != NULL) {
        struct dirent *entry;

        while ((entry = readdir(dir)) != NULL) {
            if (strstr(entry->d_name, ".json") != NULL) {
                char *file_name = strdup(entry->d_name);
                if (!file_name) continue;

                Float_Rect temp_rect = { 0, 0, 1, 1 };
                Button *button = create_button(temp_rect, file_name, file_name, load_graph_from_json);
                add_popup_button(sim_state->popup_state, button);
            }
        }
        closedir(dir);
    }
}

void handle_escape(void *function_data) {
    (void)function_data;

    if(sim_state->popup_state != NULL) {
        sim_state->popup_state->on_cancel();
    }
}

void handle_enter(void *function_data) {
    (void)function_data;

    if(sim_state->popup_state != NULL) {
        sim_state->popup_state->on_confirm();
    }
    if (sim_state->popup_state) {
        sim_state->popup_state = clear_popup(sim_state->popup_state);
    }
}

void handle_space_pressed() {
    sim_state->is_paused = !sim_state->is_paused;
}

void screen_point_to_world(float screen_x, float screen_y, float *world_x, float *world_y) {
    *world_x = (screen_x / sim_state->camera_zoom) + sim_state->camera_x;
    *world_y = (screen_y / sim_state->camera_zoom) + sim_state->camera_y;
}

void world_point_to_screen(float world_x, float world_y, float *screen_x, float *screen_y) {
    *screen_x = (world_x - sim_state->camera_x) * sim_state->camera_zoom;
    *screen_y = (world_y - sim_state->camera_y) * sim_state->camera_zoom;
}

void screen_rect_to_world(Float_Rect *screen, Float_Rect *out_world) {
    assert(screen != NULL);
    assert(out_world != NULL);

    float world_x1, world_y1;
    float world_x2, world_y2;

    // top left corner
    screen_point_to_world(screen->x, screen->y, &world_x1, &world_y1);

    // botom right cornder
    screen_point_to_world(screen->x + screen->w, screen->y + screen->h, &world_x2, &world_y2);

    out_world->x = fminf(world_x1, world_x2);
    out_world->y = fminf(world_y1, world_y2);
    out_world->w = fabsf(world_x2 - world_x1);
    out_world->h = fabsf(world_y2 - world_y1);
}

void world_rect_to_screen(Float_Rect *world, Float_Rect *out_screen) {
    assert(world != NULL);
    assert(out_screen != NULL);

    float screen_x1, screen_y1;
    float screen_x2, screen_y2;

    world_point_to_screen(world->x, world->y, &screen_x1, &screen_y1);
    world_point_to_screen(world->x + world->w, world->y + world->h, &screen_x2, &screen_y2);

    out_screen->x = fminf(screen_x1, screen_x2);
    out_screen->y = fminf(screen_y1, screen_y2);
    out_screen->w = fabsf(screen_x2 - screen_x1);
    out_screen->h = fabsf(screen_y2 - screen_y1);
}

