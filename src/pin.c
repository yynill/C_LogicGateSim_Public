
#include "pin.h"

int next_pin_id = 0;

Pin* create_pin(int ii, Node* parent_node) {
    Pin* p = malloc(sizeof(Pin));
    assert(p != NULL);

    p->x = 0; // will be set with reposition_node_pins()
    p->y = 0;
    p->is_input = ii;
    p->parent_node = parent_node;
    p->state = 0;
    p->connected_connections = array_create(1);
    p->id = next_pin_id++;

    return p;
}

Pin *find_pin_by_id(DynamicArray *nodes, int id) {
    assert(nodes != NULL);

    if (id >= next_pin_id) return NULL;

    for (int i = 0; i < nodes->size; i++) {
        Node *node = array_get(nodes, i);

        if (node->sub_nodes != NULL) {
            Pin *found = find_pin_by_id(node->sub_nodes, id);
            if (found && found->id == id) return found;
        }

        for (int j = 0; j < node->inputs->size; j++) {
            Pin *i_pin = array_get(node->inputs, j);
            if (i_pin->id == id) return i_pin;
        }

        for (int j = 0; j < node->outputs->size; j++) {
            Pin *o_pin = array_get(node->outputs, j);
            if (o_pin->id == id) return o_pin;
        }
    }

    return NULL;
}

Pin* find_corresponding_pin(Pin *original_pin, DynamicArray *original_nodes, DynamicArray *pasted_nodes) {
    for (int i = 0; i < original_nodes->size; i++) {
        Node *original_node = array_get(original_nodes, i);

        for (int j = 0; j < original_node->inputs->size; j++) {
            Pin *pin = array_get(original_node->inputs, j);
            if (pin == original_pin) {
                Node *pasted_node = array_get(pasted_nodes, i);
                return array_get(pasted_node->inputs, j);
            }
        }

        for (int j = 0; j < original_node->outputs->size; j++) {
            Pin *pin = array_get(original_node->outputs, j);
            if (pin == original_pin) {
                Node *pasted_node = array_get(pasted_nodes, i);
                return array_get(pasted_node->outputs, j);
            }
        }
    }
    return NULL;
}

void print_pin(Pin *p) {
    if (!p) {
        printf("Pin: (null)\n");
        return;
    }

    printf("=== Pin ID: %d ===\n", p->id);
    printf(" Position: (%f, %f)\n", p->x, p->y);
    printf(" Is Input: %s\n", p->is_input ? "Yes" : "No");
    printf(" State:    %d\n", p->state);
    if (p->parent_node) {
        printf(" Parent:   Node '%s' [%p]\n", p->parent_node->name, (void*)p->parent_node);
    } else {
        printf(" Parent:   (null)\n");
    }
    printf("===================\n");
}
