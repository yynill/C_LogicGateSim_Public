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
    int node_height = fmaxf(fmaxf(inputs_height, outputs_height), NODE_HEIGHT);

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
    assert(name != NULL);

    Node *node = malloc(sizeof(Node));
    if (!node) return NULL;

    Float_Rect rect = calc_rect(spawn_pos, num_inputs, num_outputs, name);
    node->rect = rect;
    node->operation = op;
    node->name = strdup(name && name[0] ? name : "NULL");
    node->sub_nodes = NULL;
    node->sub_connections = NULL;
    node->parent = NULL;
    node->input_mappings = NULL;
    node->output_mappings = NULL;

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

    for (int i = 0; i < num_inputs; i++) {
        Pin *p = create_pin(1, node);
        array_add(node->inputs, p);
    }
    for (int i = 0; i < num_outputs; i++) {
        Pin *p = create_pin(0, node);
        array_add(node->outputs, p);
    }

    reposition_node_pins(node);

    run_node(node);

    return node;
}

Node *create_group_node(SDL_Point *spawn_pos, int num_inputs, int num_outputs, const char *name, DynamicArray *sub_nodes, DynamicArray *sub_connections) {
    assert(sub_nodes != NULL);
    assert(sub_connections != NULL);

    char *name_copy = strdup(name);
    Node *group_node = create_node(num_inputs, num_outputs, nullGate, spawn_pos, name_copy);

    group_node->sub_nodes = array_create(8);
    group_node->sub_connections = array_create(8);

    array_move_all(group_node->sub_nodes, sub_nodes);
    array_move_all(group_node->sub_connections, sub_connections);

    for (int i = 0; i < group_node->sub_nodes->size; i++) {
        Node *sub_node = array_get(group_node->sub_nodes, i);
        sub_node->parent = group_node;
        DynamicArray *current_nodes = get_current_node_layer();
        array_remove(current_nodes, sub_node);
    }

    for (int i = 0; i < group_node->sub_connections->size; i++) {
        Connection *sub_con = array_get(group_node->sub_connections, i);
        DynamicArray *current_connections = get_current_connection_layer();
        array_remove(current_connections, sub_con);
    }

    group_node->input_mappings = array_create(8);
    group_node->output_mappings = array_create(8);

    create_pin_mappings(group_node);

    return group_node;
}

Node *copy_node(Node *node) {
    SDL_Point point = {node->rect.x + 10, node->rect.y + 10};
    SDL_Point *pos = &point;

    if (node->sub_nodes == NULL) {
        printf("🔵copy_node\n");
        print_node(node);
        for (int i = 0; i < node->inputs->size; i++) {
            Pin *p = array_get(node->inputs, i);
            printf("🔵copy_node - input %d: %d\n", i, p->id);
        }
        for (int i = 0; i < node->outputs->size; i++) {
            Pin *p = array_get(node->outputs, i);
            printf("🔵copy_node - output %d: %d\n", i, p->id);
        }

        int num_inputs = node->inputs->size;
        int num_outputs = node->outputs->size;

        Node *node_copy = create_node(num_inputs, num_outputs, node->operation, pos, node->name);

        return node_copy;
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

    int num_inputs = node->inputs->size;
    int num_outputs = node->outputs->size;

    Node *group_node_copy = create_group_node(pos, num_inputs, num_outputs, node->name, copied_sub_nodes, copied_sub_connections);

    return group_node_copy;
}

void move_node(Node *node, float dx, float dy) {
    node->rect.x += dx;
    node->rect.y += dy;

    if (node->sub_connections != NULL) {
        for (int j = 0; j < node->sub_connections->size; j++) {
            Connection *con = array_get(node->sub_connections, j);
            for (int i = 0; i < con->points->size; i++) {
                Connection_point *pt = array_get(con->points, i);
                pt->pos.x += dx;
                pt->pos.y += dy;
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

SDL_Point calculate_pos_for_group_node(DynamicArray *sub_nodes, DynamicArray *sub_connections){
    SDL_Point point;

    if (sub_nodes->size == 0 && sub_connections->size == 0) {
        return (SDL_Point){0, 0};
    }

    Node* first = array_get(sub_nodes, 0);

    int lowest_x = first->rect.x;
    int highest_x = first->rect.x + first->rect.w;
    int lowest_y = first->rect.y;
    int highest_y = first->rect.y + first->rect.h;

    for (int i = 0; i < sub_nodes->size; i++) {
        Node* current = array_get(sub_nodes, i);

        if (current->rect.x < lowest_x) lowest_x = current->rect.x;
        if (current->rect.y < lowest_y) lowest_y = current->rect.y;
        if (current->rect.x + current->rect.w > highest_x) highest_x = current->rect.x + current->rect.w;
        if (current->rect.y + current->rect.h > highest_y) highest_y = current->rect.y + current->rect.h;
    }

    for (int i = 0; i < sub_connections->size; i++) {
        Connection* con = array_get(sub_connections, i);
        for (int j = 0; j < con->points->size; j++) {
            Connection_point *p = array_get(con->points, j);
            if (p->pos.x < lowest_x) lowest_x = p->pos.x;
            if (p->pos.y < lowest_y) lowest_y = p->pos.y;
            if (p->pos.x > highest_x) highest_x = p->pos.x;
            if (p->pos.y > highest_y) highest_y = p->pos.y;
        }
    }

    Float_Rect outline_rect;
    outline_rect.x = lowest_x - PADDING;
    outline_rect.y = lowest_y - PADDING;
    outline_rect.w = (highest_x - lowest_x) + 2 * PADDING;
    outline_rect.h = (highest_y - lowest_y) + 2 * PADDING;

    point.x = outline_rect.x + (outline_rect.w / 2);
    point.y = outline_rect.y + (outline_rect.h / 2);

    return point;
}

void free_node(Node *node) {
    assert(node != NULL);

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
        if (node->input_mappings) {
            for (int i = 0; i < node->input_mappings->size; i++) {
                PinMapping *mapping = array_get(node->input_mappings, i);
                if (mapping != NULL && mapping->inner_pin != NULL && mapping->outer_pin != NULL) {
                    mapping->inner_pin->state = mapping->outer_pin->state;
                }
            }
        }

        for (int i = 0; i < node->sub_nodes->size; i++) {
            Node *sub_node = array_get(node->sub_nodes, i);
            run_node(sub_node);
        }

        if (node->sub_connections) {
            for (int i = 0; i < node->sub_connections->size; i++) {
                Connection *sub_con = array_get(node->sub_connections, i);
                propagate_state(sub_con);
            }
        }

        if (node->output_mappings) {
            for (int i = 0; i < node->output_mappings->size; i++) {
                PinMapping *mapping = array_get(node->output_mappings, i);
                if (mapping != NULL && mapping->inner_pin != NULL && mapping->outer_pin != NULL) {
                    mapping->outer_pin->state = mapping->inner_pin->state;
                }
            }
        }
    }
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
        printf("  This is a Group Node (%d sub-nodes)\n", sub_node_count);

        for (int i = 0; i < sub_node_count; i++) {
            Node *sub_node = array_get(node->sub_nodes, i);
            printf("    Sub-Node %d: %s (%p)\n", i, sub_node->name, (void *)sub_node);
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


PinMapping *create_pin_mapping(Pin *outer_pin, Pin *inner_pin) {
    PinMapping *mapping = malloc(sizeof(PinMapping));
    if (!mapping) return NULL;

    mapping->outer_pin = outer_pin;
    mapping->inner_pin = inner_pin;

    return mapping;
}

void create_pin_mappings(Node *group_node) {
    if (!group_node || !group_node->sub_nodes) return;

    if (group_node->input_mappings == NULL) {
        group_node->input_mappings = array_create(8);
    }
    if (group_node->output_mappings == NULL) {
        group_node->output_mappings = array_create(8);
    }

    int switch_index = 0;
    int light_index = 0;

    for (int i = 0; i < group_node->sub_nodes->size; i++) {
        Node *sub_node = array_get(group_node->sub_nodes, i);
        if (sub_node == NULL) continue;

        if (sub_node->operation == switchNode && switch_index < group_node->inputs->size) {
            Pin *outer_pin = array_get(group_node->inputs, switch_index);
            Pin *inner_pin = array_get(sub_node->outputs, 0);

            if (outer_pin != NULL && inner_pin != NULL) {
                PinMapping *mapping = create_pin_mapping(outer_pin, inner_pin);
                if (mapping != NULL) {
                    array_add(group_node->input_mappings, mapping);
                }
            }

            switch_index++;
        }

        if (sub_node->operation == lightNode && light_index < group_node->outputs->size) {
            Pin *outer_pin = array_get(group_node->outputs, light_index);
            Pin *inner_pin = array_get(sub_node->inputs, 0);

            if (outer_pin != NULL && inner_pin != NULL) {
                PinMapping *mapping = create_pin_mapping(outer_pin, inner_pin);
                if (mapping != NULL) {
                    array_add(group_node->output_mappings, mapping);
                }
            }

            light_index++;
        }
    }
}

PinMapping *find_pin_mapping(Node *node, Pin *pin){
    if (pin == NULL) return NULL;

    if (node->input_mappings != NULL) {
        for (int i = 0; i < node->input_mappings->size; i++) {
            PinMapping *mapping = array_get(node->input_mappings, i);
            if (mapping != NULL && mapping->inner_pin == pin) return mapping;
            if (mapping != NULL && mapping->outer_pin == pin) return mapping;
        }
    }
    if (node->output_mappings != NULL) {
        for (int i = 0; i < node->output_mappings->size; i++) {
            PinMapping *mapping = array_get(node->output_mappings, i);
            if (mapping != NULL && mapping->inner_pin == pin) return mapping;
            if (mapping != NULL && mapping->outer_pin == pin) return mapping;
        }
    }
    return NULL;
}

void add_pin_mapping(Node *added_node) {
    Node *parent = added_node->parent;
    if (parent == NULL) return;
    if (parent->sub_nodes == NULL) return;

    if (added_node->operation == switchNode) {
        Pin *new_pin = create_pin(1, parent);
        array_add(parent->inputs, new_pin);
        Pin *inner_pin = array_get(added_node->outputs, 0);
        PinMapping *mapping = create_pin_mapping(new_pin, inner_pin);
        array_add(parent->input_mappings, mapping);
    }
    else if (added_node->operation == lightNode) {
        Pin *new_pin = create_pin(0, parent);
        array_add(parent->outputs, new_pin);
        Pin *inner_pin = array_get(added_node->inputs, 0);
        PinMapping *mapping = create_pin_mapping(new_pin, inner_pin);
        array_add(parent->output_mappings, mapping);
    }

    reposition_node_pins(parent);
}

void remove_pin_mapping(Node *removed_node) {
    Node *parent = removed_node->parent;
    if (parent == NULL) return;
    if (parent->sub_nodes == NULL) return;

    if (removed_node->operation == switchNode) {
        Pin *inner_pin = array_get(removed_node->outputs, 0);
        PinMapping *mapping = find_pin_mapping(parent, inner_pin);
        Pin *outer_pin = mapping->outer_pin;

        if (outer_pin == NULL) return;

        for (int i = 0; i < outer_pin->connected_connections->size; i++) {
            Connection *con = array_get(outer_pin->connected_connections, i);
            Connection_point *point = find_connection_point_with_pin(con, outer_pin);
            DynamicArray *connection_layer = get_connection_layer_of_node(parent);
            delete_connection_branch(point, connection_layer);
        }

        array_remove(parent->input_mappings, mapping);
        array_remove(parent->inputs, outer_pin);
    }
    else if (removed_node->operation == lightNode) {
        Pin *inner_pin = array_get(removed_node->inputs, 0);
        PinMapping *mapping = find_pin_mapping(parent, inner_pin);
        Pin *outer_pin = mapping->outer_pin;

        if (outer_pin == NULL) return;

        for (int i = 0; i < outer_pin->connected_connections->size; i++) {
            Connection *con = array_get(outer_pin->connected_connections, i);
            Connection_point *point = find_connection_point_with_pin(con, outer_pin);
            DynamicArray *connection_layer = get_connection_layer_of_node(parent);
            delete_connection_branch(point, connection_layer);
        }

        array_remove(parent->output_mappings, mapping);
        array_remove(parent->outputs, outer_pin);
    }

    reposition_node_pins(parent);
}

void reposition_node_pins(Node *node) {
    if (!node || !node->inputs || !node->outputs) return;

    Float_Rect rect = node->rect;
    int num_inputs = node->inputs->size;
    int num_outputs = node->outputs->size;

    // Update rectangle size
    SDL_Point pos = {.x = node->rect.x, .y = node->rect.y};
    Float_Rect new_rect = calc_rect(&pos, num_inputs, num_outputs, node->name);
    node->rect = new_rect;

    float spacing = 0.5f * PIN_SIZE;

    if (num_inputs > 0) {
        float total_inputs_height = num_inputs * PIN_SIZE + (num_inputs - 1) * spacing;
        float start_y_inputs = (node->rect.h / 2.0f) - (total_inputs_height / 2.0f);

        for (int i = 0; i < num_inputs; i++) {
            Pin *pin = array_get(node->inputs, i);
            pin->x = -PIN_SIZE / 2;
            pin->y = start_y_inputs + i * (PIN_SIZE + spacing);
        }
    }

    if (num_outputs > 0) {
        float total_outputs_height = num_outputs * PIN_SIZE + (num_outputs - 1) * spacing;
        float start_y_outputs = (node->rect.h / 2.0f) - (total_outputs_height / 2.0f);

        for (int i = 0; i < num_outputs; i++) {
            Pin *pin = array_get(node->outputs, i);
            pin->x = rect.w - PIN_SIZE / 2;
            pin->y = start_y_outputs + i * (PIN_SIZE + spacing);
        }
    }
}
