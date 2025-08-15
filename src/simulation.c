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

    DynamicArray *inputs = array_create_empty_with_size(num_inputs);
    DynamicArray *outputs = array_create_empty_with_size(num_outputs);

    array_add(sim_state->nodes, create_node(inputs, outputs, op, &node_pos, button->name));

    array_free(inputs);
    array_free(outputs);
}

void cut_connection() {
    assert(sim_state->connections != NULL);
    assert(sim_state->knife_stroke != NULL);


    if (sim_state->knife_stroke->size < 2) {
        goto end;
    }

    for (int i = 0; i < sim_state->connections->size; i++) {
        Connection *con = array_get(sim_state->connections, i);
        if (con == NULL || con->points == NULL || con->points->size < 2) continue;

        for (int j = 0; j < con->points->size; j++) {
            Connection_point *con_p1 = array_get(con->points, j);

            for (int k = 0; k < con_p1->neighbors->size; k++) {
                Connection_point *con_p2 = array_get(con_p1->neighbors, k);
                if (con_p1 < con_p2) continue;

                for (int l = 0; l < sim_state->knife_stroke->size - 1; l++) {
                    SDL_Point *knife_p1 = array_get(sim_state->knife_stroke, l);
                    SDL_Point *knife_p2 = array_get(sim_state->knife_stroke, l + 1);

                    SDL_Point intersection;
                    if (segment_intersection(knife_p1, knife_p2, &(SDL_Point){con_p1->x, con_p1->y}, &(SDL_Point){con_p2->x, con_p2->y}, &intersection)) {
                        unmerge_connection(con, con_p1, con_p2);
                        goto end;
                    }
                }
            }
        }
    }

    end:
    array_free(sim_state->knife_stroke);
    sim_state->knife_stroke = array_create(16);
    sim_state->is_knife_dragging = 0;
}

void toggle_play_pause(void *function_data) {
    (void)function_data;

    sim_state->is_paused = !sim_state->is_paused;
}

void one_step(void *function_data) {

    (void)function_data;

    for (int i = 0; i < sim_state->nodes->size; i++) {
        Node *node = array_get(sim_state->nodes, i);
        run_node(node);
    }

    for (int i = 0; i < sim_state->connections->size; i++) {
        Connection *con = array_get(sim_state->connections, i);
        propagate_state(con);
    }
}

int try_handle_node_left_click() {
    float world_x, world_y;
    screen_point_to_world(sim_state->mouse_x, sim_state->mouse_y, &world_x, &world_y);

    Node *node = find_node_at_position(sim_state->nodes, world_x, world_y);
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
    sim_state->drag_offset_x = world_x - sim_state->hovered_connection_point->x;
    sim_state->drag_offset_y = world_y - sim_state->hovered_connection_point->y;
    return 1;
}

int try_handle_connection_right_click() {
    float world_x, world_y;
    screen_point_to_world(sim_state->mouse_x, sim_state->mouse_y, &world_x, &world_y);

    Connection *closest_connection = NULL;
    Connection_point *closest_p1 = NULL;
    Connection_point *closest_p2 = NULL;
    float closest_distance = 10.0f;

    for (int i = 0; i < sim_state->connections->size; i++) {
        Connection *con = array_get(sim_state->connections, i);

        for (int j = 0; j < con->points->size; j++) {
            Connection_point *p1 = array_get(con->points, j);

            for (int k = 0; k < p1->neighbors->size; k++) {
                Connection_point *p2 = array_get(p1->neighbors, k);

                if (p1 < p2) {
                    float distance = distance_to_line_segment(world_x, world_y, p1->x, p1->y, p2->x, p2->y);

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

int try_handle_close_group_button_click(DynamicArray *nodes) {
    float world_x, world_y;
    screen_point_to_world(sim_state->mouse_x, sim_state->mouse_y, &world_x, &world_y);

    DynamicArray *group_buttons = array_create(4);

    for (int i = 0; i < nodes->size; i++) {
        Node *node = array_get(nodes, i);

        if (node->close_btn != NULL) {
            array_add(group_buttons, node->close_btn);
            Button *clicked_group_button = find_button_at_position(group_buttons, world_x, world_y);
            if (clicked_group_button) {
                clicked_group_button->on_press(node);
                free(group_buttons);
                return 1;
            }
            else {
                array_remove_last(group_buttons);
            }
        }

        if (node->sub_nodes != NULL) {
            if (try_handle_close_group_button_click(node->sub_nodes)) {
                free(group_buttons);
                return 1;
            }
        }
    }
    free(group_buttons);
    return 0;
}

int try_handle_button_click() {
    Button *clicked_button = find_button_at_position(sim_state->buttons, sim_state->mouse_x, sim_state->mouse_y);

    if (clicked_button) {
        clicked_button->on_press(clicked_button);
        return 1;
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
        pt->x += offset_x;
        pt->y += offset_y;
    }

    update_all_connections(sim_state->connections, sim_state->nodes);
    return 1;
}

DynamicArray* get_all_connection_points() {
    DynamicArray *result = array_create(16);

    for (int i = 0; i < sim_state->connections->size; i++) {
        Connection *con = array_get(sim_state->connections, i);
        for (int j = 0; j < con->points->size; j++) {
            Connection_point *point = array_get(con->points, j);
            array_add(result, point);
        }
    }

    for (int i = 0; i < sim_state->nodes->size; i++) {
        Node *node = array_get(sim_state->nodes, i);
        if (node->sub_connections && node->is_expanded == 1) {
            for (int c = 0; c < node->sub_connections->size; c++) {
                Connection *con = array_get(node->sub_connections, c);
                for (int p = 0; p < con->points->size; p++) {
                    Connection_point *point = array_get(con->points, p);
                    array_add(result, point);
                }
            }
        }
    }

    return result;
}

int try_handle_connection_point_dragging(float world_x, float world_y) {
    if (!sim_state->is_connection_point_dragging || !sim_state->dragging_connection_point) return 0;

    sim_state->hovered_connection_point = sim_state->dragging_connection_point;

    sim_state->dragging_connection_point->x = world_x - sim_state->drag_offset_x - CONNECTION_POINT_SIZE;
    sim_state->dragging_connection_point->y = world_y - sim_state->drag_offset_y - CONNECTION_POINT_SIZE;
    update_connection_geometry(sim_state->dragging_connection_point->parent_connection);

    return 1;
}

int try_hover_connection_point(float world_x, float world_y) {
    DynamicArray *connection_points = get_all_connection_points();

    for (int i = 0; i < connection_points->size; i++) {
        Connection_point *point = array_get(connection_points, i);

        int dx = world_x - point->x;
        int dy = world_y - point->y;
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
    for (int i = 0; i < sim_state->nodes->size; i++) {
        Node *node = array_get(sim_state->nodes, i);

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

Node *find_node_at_position(DynamicArray *nodes, float x, float y) {
    for (int i = nodes->size - 1; i >= 0 ; i--) {
        Node *node = array_get(nodes, i);

        if (node->sub_nodes && node->sub_nodes->size > 0 && node->is_expanded) {
            Node *sub_node = find_node_at_position(node->sub_nodes, x, y);
            if (sub_node) return sub_node;
        }

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

        if (!array_contains(sim_state->connections, sim_state->new_connection)) {
            array_add(sim_state->connections, sim_state->new_connection);
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
    for (int i = 0; i < node->inputs->size; i++) {
        Pin *i_pin = array_get(node->inputs, i);
        for (int j = 0; j < i_pin->connected_connections->size; j++) {
            Connection *con = array_get(i_pin->connected_connections, j);
            Connection_point *point = find_connection_point_with_pin(con, i_pin);
            delete_connection_branch(point, i_pin);
        }
    }

    for (int i = 0; i < node->outputs->size; i++) {
        Pin *o_pin = array_get(node->outputs, i);
        for (int j = 0; j < o_pin->connected_connections->size; j++) {
            Connection *con = array_get(o_pin->connected_connections, j);
            Connection_point *point = find_connection_point_with_pin(con, o_pin);
            delete_connection_branch(point, o_pin);
        }
    }

    if (node->sub_nodes != NULL) {
        for (int i = 0; i < node->sub_nodes->size; i ++) {
            Node *sub_node = array_get(node->sub_nodes, i);
            delete_node_and_connections(sub_node);
        }
        array_free(node->sub_nodes);
        node->sub_nodes = NULL;
    }

    if (node->parent != NULL) {
        if (array_contains(node->parent->sub_nodes, node)) {
            array_remove(node->parent->sub_nodes, node);
        }
    }
    else {
        if (array_contains(sim_state->nodes, node)) {
            array_remove(sim_state->nodes, node);
        }
    }

    free_node(node);
}

int try_handle_selection() {
    assert(sim_state->nodes != NULL);

    if (sim_state->is_selection_box_drawing == 0) return 0;

    Float_Rect selection_box_world;
    screen_rect_to_world(&sim_state->selection_box, &selection_box_world);

    for (int i = 0; i < sim_state->nodes->size; i++) {
        Node *node = array_get(sim_state->nodes, i);
        SDL_Rect selection_box_rect = float_rect_to_sdl_rect(&selection_box_world);
        SDL_Rect node_rect = float_rect_to_sdl_rect(&node->rect);
        if (SDL_HasIntersection(&selection_box_rect, &node_rect)) {
            array_add(sim_state->selected_nodes, node);
        }
    }

    for (int i = 0; i < sim_state->connections->size; i++) {
        Connection *con = array_get(sim_state->connections, i);
        for (int j = 0; j < con->points->size; j++) {
            Connection_point *current = array_get(con->points, j);
            if (point_in_rect(current->x, current->y, selection_box_world)) {
                array_add(sim_state->selected_connection_points, current);
            }
        }
    }

    sim_state->is_selection_box_drawing = 0;
    sim_state->selection_box = (Float_Rect){0, 0, 0, 0};
    return 1;
}

void rename_note() {
    if (sim_state->selected_nodes->size != 1) {
        printf("❌ You can only rename 1 note at a time - select only 1\n");
        return;
    }

    Node *note_to_rename = array_get(sim_state->selected_nodes, 0);
    if (note_to_rename->name != NULL) {
        free(note_to_rename->name);
    }

    note_to_rename->name = strdup(sim_state->popup_state->name_input.text);
    return;
}

int try_handle_node_right_click() {
    float world_x, world_y;
    screen_point_to_world(sim_state->mouse_x, sim_state->mouse_y, &world_x, &world_y);

    Node *clicked_node = find_node_at_position(sim_state->nodes, world_x, world_y);
    if (clicked_node == NULL) return 0;

    if (clicked_node->operation == switchNode) {
        toggle_switch_outputs(clicked_node);
        return 1;
    }
    else if (clicked_node->sub_nodes != NULL) {
        clicked_node->is_expanded = 1;
        move_group_node_pins(clicked_node);
        return 1;
    }

    return 0;
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

    if (sim_state->popup_state != NULL) {
        if(try_handle_popup()) return;
    }
    if (try_handle_button_click()) return;
    if (try_handle_close_group_button_click(sim_state->nodes)) return;
    if (try_handle_pin_click()) return;
    if (try_handle_node_left_click()) return;
    if (try_handle_connection_point_left_click()) return;

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

    if (try_hover_connection_point(world_x, world_y)) return;
    if (try_handle_knife_stroke_motion(world_x, world_y)) return;
    if (try_update_pin_hover(world_x, world_y)) return;
    if (try_handle_node_dragging(world_x, world_y)) return;
    if (try_handle_connection_point_dragging(world_x, world_y)) return;
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
        print_node(current);
        SDL_Point pos = {.x = current->rect.x + 10, .y = current->rect.y + 10};
        DynamicArray *inputs = array_create_empty_with_size(current->inputs->size);
        DynamicArray *outputs = array_create_empty_with_size(current->outputs->size);
        Node *new;

        if (current->sub_nodes != NULL) {
            new = copy_node(current);
        }
        else {
            new = create_node(inputs, outputs, current->operation, &pos, current->name);
        }

        array_add(sim_state->nodes, new);
        array_add(sim_state->selected_nodes, new);
        array_free(inputs);
        array_free(outputs);
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

            array_add(sim_state->connections, new_con);
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
        if(node->operation == switchNode)  num_inputs++;
        if(node->operation == lightNode)  num_outputs++;
    }

    if (num_inputs == 0 || num_outputs == 0) {
        printf("Error: include inputs and outputs!\n");
        return;
    }

    DynamicArray *matching_connections = find_fully_selected_connections(sim_state->selected_nodes);

    Float_Rect outline_rect = calculate_outline_rect(sim_state->selected_nodes, matching_connections);
    Float_Rect rect = calc_rect(&((SDL_Point){0, 0}), num_inputs, num_outputs, sim_state->popup_state->name_input.text);
    SDL_Point pos = calculate_pos_from_outline_rect(outline_rect, rect);

    DynamicArray *inputs = array_create_empty_with_size(num_inputs);
    DynamicArray *outputs = array_create_empty_with_size(num_outputs);

    Node *group_node = create_group_node(&pos, inputs, outputs, sim_state->popup_state->name_input.text, sim_state->selected_nodes, matching_connections, 1);

    sim_state->selected_connection_points->size = 0;
    sim_state->selected_nodes->size = 0;

    array_add(sim_state->nodes, group_node);
    free(matching_connections);
    array_free(inputs);
    array_free(outputs);
}

void handle_r_pressed() {
    sim_state->popup_state = init_popupstate("Rename Note Node", rename_note);
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
