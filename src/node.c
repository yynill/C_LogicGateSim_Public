#include "node.h"

Float_Rect calc_rect(SDL_Point *spawn_pos, int num_inputs, int num_outputs, const char *name) {
    Float_Rect rect;

    if (strcmp(name, "SWITCH") == 0 || strcmp(name, "LIGHT") == 0) {
        rect.x = spawn_pos->x;
        rect.y = spawn_pos->y;
        rect.w = SMALL_NODE_WIDTH;
        rect.h = NODE_HEIGHT;
        return rect;
    }

    int inputs_height = (int)(PIN_SIZE * num_inputs + (num_inputs - 1) * (0.5f * PIN_SIZE)) + 2 * PADDING;
    int outputs_height = (int)(PIN_SIZE * num_outputs + (num_outputs - 1) * (0.5f * PIN_SIZE)) + 2 * PADDING;
    int node_height = fmaxf(inputs_height, outputs_height);

    int char_width = 10;
    int name_length = strlen(name);
    int text_width = char_width * name_length + 2 * PADDING;
    int node_width = fmaxf(text_width, 100);

    rect.x = spawn_pos->x;
    rect.y = spawn_pos->y;
    rect.w = node_width;
    rect.h = node_height;

    return rect;
}

Node *create_node(DynamicArray* inputs, DynamicArray* ouptuts, Operation *op, SDL_Point *spawn_pos, const char *name) {
    assert(name != NULL);

    int num_inputs = inputs->size;
    int num_outputs = ouptuts->size;

    Node *node = malloc(sizeof(Node));
    if (!node) return NULL;

    Float_Rect rect = calc_rect(spawn_pos, num_inputs, num_outputs, name);

    node->inputs = array_create(2);
    if (!node->inputs) {
        free_node(node);
        return NULL;
    }

    node->outputs = array_create(2);
    if (!node->outputs) {
        array_free(node->inputs);
        free_node(node);
        return NULL;
    }

    float spacing = 0.5f * PIN_SIZE;
    float total_inputs_height = num_inputs * PIN_SIZE + (num_inputs - 1) * spacing;
    float start_y_inputs = (rect.h / 2.0f) - (total_inputs_height / 2.0f);

    for (int i = 0; i < num_inputs; i++) {
        int id = *(int *)array_get(inputs, i);
        Pin *p = create_pin(-PIN_SIZE / 2, start_y_inputs + i * (PIN_SIZE + spacing), 1, node, id);
        array_add(node->inputs, p);
    }

    float total_outputs_height = num_outputs * PIN_SIZE + (num_outputs - 1) * spacing;
    float start_y_outputs = (rect.h / 2.0f) - (total_outputs_height / 2.0f);

    for (int i = 0; i < num_outputs; i++) {
        int id = *(int *)array_get(ouptuts, i);
        Pin *p = create_pin(rect.w - PIN_SIZE / 2, start_y_outputs + i * (PIN_SIZE + spacing), 0, node, id);
        array_add(node->outputs, p);
    }

    node->operation = op;
    node->name = strdup(name && name[0] ? name : "NULL");
    node->rect = rect;
    node->is_expanded = 0;
    node->sub_nodes = NULL;
    node->sub_connections = NULL;
    node->parent = NULL;
    node->close_btn = NULL;

    run_node(node);

    return node;
}

Node *create_group_node(SDL_Point *spawn_pos, DynamicArray* inputs, DynamicArray* ouptuts, const char *name, DynamicArray *sub_nodes, DynamicArray *sub_connections, int is_expanded) {
    assert(sub_nodes != NULL);
    assert(sub_connections != NULL);

    char *name_copy = strdup(name);
    Node *group_node = create_node(inputs, ouptuts, nullGate, spawn_pos, name_copy);

    group_node->sub_nodes = array_create(8);
    group_node->sub_connections = array_create(8);

    array_move_all(group_node->sub_nodes, sub_nodes);
    array_move_all(group_node->sub_connections, sub_connections);

    for (int i = 0; i < group_node->sub_nodes->size; i++) {
        Node *sub_node = array_get(group_node->sub_nodes, i);
        sub_node->parent = group_node;
        array_remove(sim_state->nodes, sub_node);
    }

    for (int i = 0; i < group_node->sub_connections->size; i++) {
        Connection *sub_con = array_get(group_node->sub_connections, i);
        array_remove(sim_state->connections, sub_con);
    }

    group_node->is_expanded = is_expanded;
    Float_Rect outline_rect = calculate_outline_rect(group_node->sub_nodes, group_node->sub_connections);
    group_node->outline_rect = outline_rect;
    group_node->close_btn = create_button((Float_Rect){outline_rect.x, outline_rect.y - 30, 20, 20}, "X", group_node, handle_close_group);

    move_group_node_pins(group_node);

    return group_node;
}

Node *copy_node(Node *node) {
    SDL_Point point = {node->rect.x + 10, node->rect.y + 10};
    SDL_Point *pos = &point;

    if (node->sub_nodes == NULL) {
        return create_node(node->inputs, node->outputs, node->operation, pos, node->name);
    }

    DynamicArray *copied_sub_nodes = array_create(8);
    for (int i = 0; i < node->sub_nodes->size; i++) {
        Node *sub_node = array_get(node->sub_nodes, i);
        array_add(copied_sub_nodes, copy_node(sub_node));
    }

    DynamicArray *copied_sub_connections = array_create(8);
    for (int i = 0; i < node->sub_connections->size; i++) {
        Connection *sub_con = array_get(node->sub_connections, i);
        array_add(copied_sub_connections, copy_connection(sub_con, node->sub_nodes, copied_sub_nodes, 10, 10));
    }

    Node *copy = create_group_node(pos, node->inputs, node->outputs, node->name, copied_sub_nodes, copied_sub_connections, node->is_expanded);
    return copy;
}

void move_group_node_pins(Node *node) {
    if(node->is_expanded){
        for (int i = 0; i < node->inputs->size; i++) {
            Pin *p = array_get(node->inputs, i);
            p->x = -(node->outline_rect.w / 2) + (node->rect.w / 2) - (PIN_SIZE / 2);
        }
        for (int i = 0; i < node->outputs->size; i++) {
            Pin *p = array_get(node->outputs, i);
            p->x = (node->outline_rect.w / 2) + (node->rect.w / 2) - (PIN_SIZE / 2);
        }
    }
    else {
        for (int i = 0; i < node->inputs->size; i++) {
            Pin *p = array_get(node->inputs, i);
            p->x = -(PIN_SIZE / 2);
        }
        for (int i = 0; i < node->outputs->size; i++) {
            Pin *p = array_get(node->outputs, i);
            p->x = node->rect.w - (PIN_SIZE / 2);
        }
    }

    for (int i = 0; i < node->inputs->size; i++) {
        Pin *p = array_get(node->inputs, i);
        for (int j = 0; j < p->connected_connections->size; j++) {
            Connection *con = array_get(p->connected_connections, j);
            update_connection_geometry(con);
        }
    }

    for (int i = 0; i < node->outputs->size; i++) {
        Pin *p = array_get(node->outputs, i);
        for (int j = 0; j < p->connected_connections->size; j++) {
            Connection *con = array_get(p->connected_connections, j);
            update_connection_geometry(con);
        }
    }
}

void handle_close_group(void *function_data) {
    assert(function_data != NULL);

    Node *node = (Node*)function_data;
    node->is_expanded = 0;
    move_group_node_pins(node);
}

void move_node(Node *node, float dx, float dy) {
    node->rect.x += dx;
    node->rect.y += dy;
    node->outline_rect.x += dx;
    node->outline_rect.y += dy;
    if (node->close_btn != NULL) {
        node->close_btn->rect.x += dx;
        node->close_btn->rect.y += dy;
    }

    Node *ancestor = node->parent;
    while (ancestor != NULL) {
        reshape_outline_box(ancestor);
        ancestor = ancestor->parent;
    }

    if (node->sub_connections != NULL) {
        for (int j = 0; j < node->sub_connections->size; j++) {
            Connection *con = array_get(node->sub_connections, j);
            for (int i = 0; i < con->points->size; i++) {
                Connection_point *pt = array_get(con->points, i);
                pt->x += dx;
                pt->y += dy;
            }
        }
    }

    if (node->sub_nodes != NULL) {
        for (int i = 0; i < node->sub_nodes->size; i++) {
            Node *current = array_get(node->sub_nodes, i);
            move_node(current, dx, dy);
        }
    }
}

Float_Rect calculate_outline_rect(DynamicArray *sub_nodes, DynamicArray *sub_connections) {
    if (sub_nodes->size == 0 && sub_connections->size == 0) {
        return (Float_Rect){0, 0, 0, 0};
    }

    Node* first = array_get(sub_nodes, 0);

    int lowest_x = first->rect.x;
    int highest_x = first->rect.x + first->rect.w;
    int lowest_y = first->rect.y;
    int highest_y = first->rect.y + first->rect.h;

    for (int i = 0; i < sub_nodes->size; i++) {
        Node* current = array_get(sub_nodes, i);

        Float_Rect inner_outline = {0, 0, 0, 0};
        if (current->sub_nodes && current->sub_nodes->size > 0 && current->is_expanded) {
            inner_outline = calculate_outline_rect(current->sub_nodes, current->sub_connections);
        }

        int nodes_lowest_x = current->rect.x;
        int nodes_highest_x = current->rect.x + current->rect.w;
        int nodes_lowest_y = current->rect.y;
        int nodes_highest_y = current->rect.y + current->rect.h;

        if (inner_outline.w > 0 && inner_outline.h > 0) {
            if (inner_outline.x < nodes_lowest_x) nodes_lowest_x = inner_outline.x;
            if (inner_outline.y < nodes_lowest_y) nodes_lowest_y = inner_outline.y;
            if (inner_outline.x + inner_outline.w > nodes_highest_x) nodes_highest_x = inner_outline.x + inner_outline.w;
            if (inner_outline.y + inner_outline.h > nodes_highest_y) nodes_highest_y = inner_outline.y + inner_outline.h;
        }

        if (nodes_lowest_x < lowest_x) lowest_x = nodes_lowest_x;
        if (nodes_lowest_y < lowest_y) lowest_y = nodes_lowest_y;
        if (nodes_highest_x > highest_x) highest_x = nodes_highest_x;
        if (nodes_highest_y > highest_y) highest_y = nodes_highest_y;
    }

    for (int i = 0; i < sub_connections->size; i++) {
        Connection* con = array_get(sub_connections, i);
        for (int j = 0; j < con->points->size; j++) {
            SDL_Point *p = array_get(con->points, j);
            if (p->x < lowest_x) lowest_x = p->x;
            if (p->y < lowest_y) lowest_y = p->y;
            if (p->x > highest_x) highest_x = p->x;
            if (p->y > highest_y) highest_y = p->y;
        }
    }

    Float_Rect outline_rect;
    outline_rect.x = lowest_x - PADDING;
    outline_rect.y = lowest_y - PADDING;
    outline_rect.w = (highest_x - lowest_x) + 2 * PADDING;
    outline_rect.h = (highest_y - lowest_y) + 2 * PADDING;

    return outline_rect;
}

SDL_Point calculate_pos_from_outline_rect(Float_Rect outline_rect, Float_Rect node_rect) {
    SDL_Point point;

    point.x = outline_rect.x + (outline_rect.w / 2) - (node_rect.w / 2);
    point.y = outline_rect.y + (outline_rect.h / 2) - (node_rect.h / 2);

    return point;
}

SDL_Point *find_most_top_left(DynamicArray *nodes) {
    if (nodes->size == 0) return NULL;

    SDL_Point *top_left = malloc(sizeof(SDL_Point));
    if (!top_left) return NULL;

    Node *first = array_get(nodes, 0);
    top_left->x = first->rect.x;
    top_left->y = first->rect.y;

    for (int i = 1; i < nodes->size; i++) {
        Node *current = array_get(nodes, i);
        if (current->rect.x < top_left->x) top_left->x = current->rect.x;
        if (current->rect.y < top_left->y) top_left->y = current->rect.y;
    }

    return top_left;
}

void free_node(Node *node) {
    assert(node != NULL);

    // printf("‼️ free_node - node %p\n", (void *)node);

    if (node->inputs != NULL) {
        array_free(node->inputs);
        node->inputs = NULL;
    }

    if (node->outputs != NULL) {
        array_free(node->outputs);
        node->outputs = NULL;
    }

    if (node->sub_nodes != NULL) {
        array_free(node->sub_nodes);
        node->sub_nodes = NULL;
    }

    // node->sub_connections deletet at delete_node_and_connections externally

    free(node);
    node = NULL;
}

void connect_subgraph_inputs(Node *node) {
    int input_index = 0;
    for (int i = 0; i < node->sub_nodes->size; i++) {
        Node *sub = array_get(node->sub_nodes, i);
        if (sub->operation == switchNode && input_index < node->inputs->size) {
            Pin *outer_pin = array_get(node->inputs, input_index);
            Pin *inner_output = array_get(sub->outputs, 0);
            inner_output->state = outer_pin->state;
            input_index++;
        }
    }
}

void propagate_subgraph_states(Node *node) {
    for (int i = 0; i < node->sub_connections->size; i++) {
        propagate_state(array_get(node->sub_connections, i));
    }
}

void connect_subgraph_outputs(Node *node) {
    int output_index = 0;
    for (int i = 0; i < node->sub_nodes->size; i++) {
        Node *sub = array_get(node->sub_nodes, i);
        if (sub->operation == lightNode && output_index < node->outputs->size) {
            Pin *outer_pin = array_get(node->outputs, output_index);
            Pin *inner_input = array_get(sub->inputs, 0);
            outer_pin->state = inner_input->state;
            output_index++;
        }
    }
}

void run_node(Node *node) {
    assert(node != NULL);
    assert(node->inputs && node->outputs && node->operation);

    if (node->inputs->size == 0 || node->outputs->size == 0) return;

    int a = ((Pin *)array_get(node->inputs, 0))->state;
    int b = (node->inputs->size > 1) ? ((Pin *)array_get(node->inputs, 1))->state : 0;

    int result = node->operation(a, b);

    for (int i = 0; i < node->outputs->size; i++) {
        ((Pin *)array_get(node->outputs, i))->state = result;
    }

    if (node->sub_nodes) {
        connect_subgraph_inputs(node);
        for (int i = 0; i < node->sub_nodes->size; i++) {
            run_node((Node *)array_get(node->sub_nodes, i));
        }
        propagate_subgraph_states(node);
        connect_subgraph_outputs(node);
    }
}

void reshape_outline_box(Node *node) {
    if(node == NULL) return;

    if (node->sub_nodes == NULL || node->sub_nodes->size == 0) return;

    Float_Rect outline = calculate_outline_rect(node->sub_nodes, node->sub_connections);

    node->outline_rect.x = outline.x;
    node->outline_rect.y = outline.y;
    node->outline_rect.w = outline.w;
    node->outline_rect.h = outline.h;

    node->rect.x = node->outline_rect.x + (node->outline_rect.w - node->rect.w) / 2;
    node->rect.y = node->outline_rect.y + (node->outline_rect.h - node->rect.h) / 2;

    node->close_btn->rect.x = node->outline_rect.x;
    node->close_btn->rect.y = node->outline_rect.y - 30;

    move_group_node_pins(node);
}

void print_node(Node *node) {
    if (!node) {
        printf("Invalid Node (NULL pointer)\n");
        return;
    }

    printf("Node (%p): %s\n", (void *)node, node->name);
    printf("  Position: (%f, %f) Size: (%f x %f)\n",
           node->rect.x, node->rect.y, node->rect.w, node->rect.h);


    if (node->sub_nodes) {
        int sub_node_count = node->sub_nodes->size;
        printf("  This is a Group Node (%d sub-nodes), %s\n",
               sub_node_count,
               node->is_expanded ? "expanded" : "collapsed");

        if (node->is_expanded) {
            for (int i = 0; i < sub_node_count; i++) {
                Node *sub_node = array_get(node->sub_nodes, i);
                printf("    Sub-Node %d: %s (%p)\n", i, sub_node->name, (void *)sub_node);
            }
        }
    }

    if (node->parent) {
        printf("  This Node has a parent (%p): %s\n", (void *)node->parent, node->parent->name);
    }

    printf("  Inputs (%d):\n", node->inputs->size);
    for (int i = 0; i < node->inputs->size; i++) {
        Pin *pin = array_get(node->inputs, i);
        printf("    [Input %d]  Ptr: %p | ID: %d | State: %d | Pos: (%f, %f)\n",
               i, (void *)pin, pin->id, pin->state, pin->x, pin->y);
        if (pin->connected_connections && pin->connected_connections->size > 0) {
            printf("      └─ Connected Connections (%d):\n", pin->connected_connections->size);
            for (int j = 0; j < pin->connected_connections->size; j++) {
                Connection *con = array_get(pin->connected_connections, j);
                printf("          • Connection Ptr: %p\n", (void*)con);
            }
        }
    }

    printf("  Outputs (%d):\n", node->outputs->size);
    for (int i = 0; i < node->outputs->size; i++) {
        Pin *pin = array_get(node->outputs, i);
        printf("    [Output %d] Ptr: %p | ID: %d | State: %d | Pos: (%f, %f)\n",
               i, (void *)pin, pin->id, pin->state, pin->x, pin->y);
        if (pin->connected_connections && pin->connected_connections->size > 0) {
            printf("      └─ Connected Connections (%d):\n", pin->connected_connections->size);
            for (int j = 0; j < pin->connected_connections->size; j++) {
                Connection *con = array_get(pin->connected_connections, j);
                printf("          • Connection Ptr: %p\n", (void*)con);
            }
        }
    }

    printf("--------------------------------\n");
}
