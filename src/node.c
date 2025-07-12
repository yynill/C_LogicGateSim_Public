#include "node.h"

SDL_Rect calc_rect(SDL_Point *spawn_pos, int num_inputs, int num_outputs, const char *name) {
    SDL_Rect rect;

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

Node *create_node(int num_inputs, int num_outputs, Operation *op, SDL_Point *spawn_pos, const char *name) {
    assert(num_inputs >= 0 && num_outputs >= 0);
    assert(name != NULL);

    Node *node = malloc(sizeof(Node));
    if (!node) return NULL;

    SDL_Rect rect = calc_rect(spawn_pos, num_inputs, num_outputs, name);

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
        Pin *p = create_pin(-PIN_SIZE / 2, start_y_inputs + i * (PIN_SIZE + spacing), 1, node, -1);
        array_add(node->inputs, p);
    }

    float total_outputs_height = num_outputs * PIN_SIZE + (num_outputs - 1) * spacing;
    float start_y_outputs = (rect.h / 2.0f) - (total_outputs_height / 2.0f);

    for (int i = 0; i < num_outputs; i++) {
        Pin *p = create_pin(rect.w - PIN_SIZE / 2, start_y_outputs + i * (PIN_SIZE + spacing), 0, node, -1);
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
}

void handle_close_group(SimulationState *state, void *function_data) {
    assert(state != NULL);
    assert(function_data != NULL);

    Node *node = (Node*)function_data;
    node->is_expanded = 0;
    move_group_node_pins(node);
}

SDL_Rect calculate_outline_rect(DynamicArray *sub_nodes, DynamicArray *sub_connections) {
    if (sub_nodes->size == 0 && sub_connections->size == 0) {
        return (SDL_Rect){0, 0, 0, 0};
    }

    Node* first = array_get(sub_nodes, 0);

    int lowest_x = first->rect.x;
    int highest_x = first->rect.x + first->rect.w;
    int lowest_y = first->rect.y;
    int highest_y = first->rect.y + first->rect.h;

    for (int i = 0; i < sub_nodes->size; i++) {
        Node* current = array_get(sub_nodes, i);

        SDL_Rect inner_outline = {0, 0, 0, 0};
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

    SDL_Rect outline_rect;
    outline_rect.x = lowest_x - PADDING;
    outline_rect.y = lowest_y - PADDING;
    outline_rect.w = (highest_x - lowest_x) + 2 * PADDING;
    outline_rect.h = (highest_y - lowest_y) + 2 * PADDING;

    return outline_rect;
}

SDL_Point calculate_pos_from_outline_rect(SDL_Rect outline_rect, SDL_Rect node_rect) {
    SDL_Point pos;

    pos.x = outline_rect.x + (outline_rect.w / 2) - (node_rect.w / 2);
    pos.y = outline_rect.y + (outline_rect.h / 2) - (node_rect.h / 2);

    return pos;
}

Node *create_group_node(SimulationState *state, SDL_Point *spawn_pos, int num_inputs, int num_outputs, const char *name, DynamicArray *sub_nodes, DynamicArray *sub_connections, int is_expanded) {
    assert(sub_nodes != NULL);
    assert(sub_connections != NULL);

    char *name_copy = strdup(name);
    Node *group_node = create_node(num_inputs, num_outputs, nullGate, spawn_pos, name_copy);

    group_node->sub_nodes = array_create(8);
    group_node->sub_connections = array_create(8);

    for (int i = 0; i < sub_nodes->size; i++) {
        Node *current = array_get(sub_nodes, i);
        Node *new = NULL;
        SDL_Point pos = {.x = current->rect.x, .y = current->rect.y};

        if (current->sub_nodes != NULL && current->sub_nodes->size > 0) {
            new = create_group_node(state, &pos, current->inputs->size, current->outputs->size, current->name, current->sub_nodes, current->sub_connections, current->is_expanded);
        } else {
            new = create_node(current->inputs->size, current->outputs->size, current->operation, &pos, current->name);
        }

        new->parent = group_node;
        array_add(group_node->sub_nodes, new);
    }

    for (int i = 0; i < sub_connections->size; i++) {
        Connection *original_conn = array_get(sub_connections, i);
        Connection *new_con = copy_connection(state, original_conn, sub_nodes, group_node->sub_nodes, 0, 0);

        if (new_con != NULL) {
            array_add(group_node->sub_connections, new_con);
        }
    }

    group_node->is_expanded = is_expanded;
    SDL_Rect outline_rect = calculate_outline_rect(group_node->sub_nodes, group_node->sub_connections);
    group_node->outline_rect = outline_rect;
    group_node->close_btn = create_button((SDL_Rect){outline_rect.x, outline_rect.y - 30, 20, 20}, "X", group_node, handle_close_group);

    move_group_node_pins(group_node);

    return group_node;
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

    SDL_Rect outline = calculate_outline_rect(node->sub_nodes, node->sub_connections);

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
    printf("  Position: (%d, %d) Size: (%d x %d)\n",
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

    // Inputs
    printf("  Inputs (%d):\n", node->inputs->size);
    for (int i = 0; i < node->inputs->size; i++) {
        Pin *pin = array_get(node->inputs, i);
        printf("    Input %d (%p) -> State: %d | Pos: (%d, %d)\n",
               i, (void *)pin, pin->state, pin->x, pin->y);
        printf("        Connected, connections\n");
        for (int i = 0; i < pin->connected_connections->size; i++) {
            Connection *con = array_get(pin->connected_connections, i);
            printf("        Con (%p)\n", (void*) con);
        }
    }

    // Outputs
    printf("  Outputs (%d):\n", node->outputs->size);
    for (int i = 0; i < node->outputs->size; i++) {
        Pin *pin = array_get(node->outputs, i);
        printf("    Output %d (%p) -> State: %d | Pos: (%d, %d)\n",
               i, (void *)pin, pin->state, pin->x, pin->y);
        printf("        Connected, connections\n");
        for (int i = 0; i < pin->connected_connections->size; i++) {
            Connection *con = array_get(pin->connected_connections, i);
            printf("        Con (%p)\n", (void*) con);
        }
    }

    printf("--------------------------------\n");
}
