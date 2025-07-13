#include "connection.h"

void add_pin(Connection *con, Pin *pin) {
    if (pin->is_input) {
        array_add(con->input_pins, pin);
    }
    else {
        array_add(con->output_pins, pin);
    }
    array_add(pin->connected_connections, con);

    return;
}

Connection *start_connection(Pin *pin) {
    assert(pin != NULL);
    Connection *con = malloc(sizeof(Connection));
    con->input_pins = array_create(1);
    con->output_pins = array_create(1);
    con->points = array_create(4);
    con->state = 0;
    add_pin(con, pin);

    return con;
}

void merge_connection(Connection *con1, Connection *con2) {
    for (int i = 0; i < con2->input_pins->size; i++) {
        Pin *pin = array_get(con2->input_pins, i);
        array_clear(pin->connected_connections);
        array_add(pin->connected_connections, con1);
    }
    array_move_all(con1->input_pins, con2->input_pins);

    for (int i = 0; i < con2->output_pins->size; i++) {
        Pin *pin = array_get(con2->output_pins, i);
        array_clear(pin->connected_connections);
        array_add(pin->connected_connections, con1);
    }
    array_move_all(con1->output_pins, con2->output_pins);

    for (int i = 0; i < con2->points->size; i++) {
        Connection_point *point = array_get(con2->points, i);
        point->parent_connection = con1;
    }
    array_move_all(con1->points, con2->points);

    array_remove(sim_state->connections, con2);
    free(con2);
}

void unmerge_connection(Connection *con, Connection_point *point1, Connection_point *point2) {
    assert(con != NULL);
    assert(point1 != NULL);
    assert(point2 != NULL);

    if (con->points->size < 3) {

        if(point1->linked_to_pin != NULL) {
            Pin *pin = point1->linked_to_pin;
            array_remove(pin->connected_connections, con);
        }
        if(point2->linked_to_pin != NULL) {
            Pin *pin = point2->linked_to_pin;
            array_remove(pin->connected_connections, con);
        }
        array_remove(sim_state->connections, con);
        free_connection(con);
        return;
    }

    remove_connection_link(point1, point2);

    DynamicArray *points_to_visit = array_create(4);
    DynamicArray *visited = array_create(4);

    array_add(points_to_visit, point1);
    array_add(visited, point1);

    while (points_to_visit->size != 0) {
        Connection_point *current = array_get(points_to_visit, points_to_visit->size - 1);
        array_remove_last(points_to_visit);
        array_add(visited, current);

        for (int i = 0; i < current->neighbors->size; i++) {
            Connection_point *neighbor = array_get(current->neighbors, i);

            if(!array_contains(visited, neighbor)) {
                array_add(points_to_visit, neighbor);
            }
        }
    }

    free(points_to_visit);

    if(visited->size == con->points->size) {
        return;
    }

    DynamicArray *disconnected_points = array_create(4);
    for (int i = 0; i < con->points->size; i++) {
        Connection_point *point = array_get(con->points, i);
        if (!array_contains(visited, point)) {
            array_add(disconnected_points, point);
        }
    }

    Connection *new_con = malloc(sizeof(Connection));
    new_con->input_pins = array_create(1);
    new_con->output_pins = array_create(1);
    new_con->points = array_create(4);
    new_con->state = con->state;

    for (int i = 0; i < disconnected_points->size; i++) {
        Connection_point *point = array_get(disconnected_points, i);

        point->parent_connection = new_con;

        array_add(new_con->points, point);
        array_remove(con->points, point);

        if (point->linked_to_pin != NULL) {
            Pin *pin = point->linked_to_pin;

            array_remove(con->input_pins, pin);
            array_remove(con->output_pins, pin);

            array_remove(pin->connected_connections, con);

            add_pin(new_con, pin);
        }
    }

    array_add(sim_state->connections, new_con);

    if(point1->neighbors->size == 0) {
        if(point1->linked_to_pin != NULL) {
            Pin *pin = point1->linked_to_pin;
            array_remove(pin->connected_connections, con);
        }
        array_remove(con->points, point1);
        free_connection_point(point1);
    }

    if(point2->neighbors->size == 0) {
        if(point2->linked_to_pin != NULL) {
            Pin *pin = point2->linked_to_pin;
            array_remove(pin->connected_connections, con);
        }
        array_remove(con->points, point2);
        free_connection_point(point2);
    }

    for (int i = new_con->points->size - 1; i >= 0; i--) {
        Connection_point *point = array_get(new_con->points, i);
        if (point->neighbors->size == 0) {
            if (point->linked_to_pin != NULL) {
                Pin *pin = point->linked_to_pin;
                array_remove(pin->connected_connections, new_con);
                array_remove(new_con->input_pins, pin);
                array_remove(new_con->output_pins, pin);
            }
            array_remove(new_con->points, point);
            free_connection_point(point);
        }
    }

    free(disconnected_points);
    free(visited);
}

void remove_connection_link(Connection_point *from, Connection_point *to) {
    assert(from != NULL);
    assert(to != NULL);

    if(from->parent_connection != to->parent_connection) return;
    if(!array_contains(from->neighbors, to)) return;
    if(!array_contains(to->neighbors, from)) return;

    array_remove(from->neighbors, to);
    array_remove(to->neighbors, from);
}

void add_connection_link(Connection_point *from, Connection_point *to) {
    array_add(from->neighbors, to);
    array_add(to->neighbors, from);

    if (from->parent_connection != to->parent_connection) {
        merge_connection(from->parent_connection, to->parent_connection);
    }
}

Connection_point *find_connection_point_with_pin(Connection *con, Pin *pin) {
    assert(con != NULL);
    assert(pin != NULL);

    for (int i = 0; i < con->points->size; i++) {
        Connection_point *point = array_get(con->points, i);
        if (point->linked_to_pin == pin) {
            return point;
        }
    }

    return NULL;
}

void delete_connection_branch(Connection_point *point, Pin* pin_of_point) {
    assert(point != NULL);

    Connection *con = point->parent_connection;
    DynamicArray *points_to_remove = array_create(4);
    DynamicArray *points_to_visit = array_create(4);
    DynamicArray *visited = array_create(4);

    array_add(points_to_visit, point);
    array_add(visited, point);

    while (points_to_visit->size != 0) {
        Connection_point *current = array_get(points_to_visit, points_to_visit->size - 1);
        array_remove_last(points_to_visit);

        if (current->neighbors->size < 3) {
            array_add(points_to_remove, current);

            for (int i = 0; i < current->neighbors->size; i++) {
                Connection_point *neighbor = array_get(current->neighbors, i);

                if (!array_contains(visited, neighbor)) {
                    array_add(points_to_visit, neighbor);
                    array_add(visited, neighbor);
                }
            }
        }
    }

    if(pin_of_point != NULL){
        array_remove(con->input_pins, pin_of_point);
        array_remove(con->output_pins, pin_of_point);
    }

    if(con->points->size == points_to_remove->size) {
        for (int i = 0; i < con->input_pins->size; i++) {
            Pin *p = array_get(con->input_pins, i);
            array_remove(p->connected_connections, con);
        }
        for (int i = 0; i < con->output_pins->size; i++) {
            Pin *p = array_get(con->output_pins, i);
            array_remove(p->connected_connections, con);
        }

        free_connection(con);
        array_remove(sim_state->connections, con);
    }
    else {
        for (int i = 0; i < points_to_remove->size; i++) {
            Connection_point *rm_point = array_get(points_to_remove, i);
            for (int j = 0; j < rm_point->neighbors->size; j++) {
                Connection_point *neighbor = array_get(rm_point->neighbors, j);
                array_remove(neighbor->neighbors, rm_point);
            }

            array_remove(con->points, rm_point);
            free_connection_point(rm_point);
        }
    }

    free(points_to_remove);
    free(points_to_visit);
    free(visited);
}


void roll_back_connection_branch(Connection *con, Connection_point *first_point, Connection_point *last_point) {
    assert(con != NULL);
    assert(first_point != NULL);
    assert(last_point != NULL);

    if(first_point->linked_to_pin != NULL) {
        Pin *pin = first_point->linked_to_pin;
        array_remove(pin->connected_connections, con);
    }
    if(last_point->linked_to_pin != NULL) {
        Pin *pin = last_point->linked_to_pin;
        array_remove(pin->connected_connections, con);
    }

    if (first_point == last_point) return;

    DynamicArray *points_to_remove = array_create(4);

    Connection_point *current = last_point;
    while (current != NULL && current != first_point) {
        array_add(points_to_remove, current);

        Connection_point *next = NULL;
        for (int i = 0; i < current->neighbors->size; i++) {
            Connection_point *neighbor = array_get(current->neighbors, i);
            if (neighbor == first_point || !array_contains(points_to_remove, neighbor)) {
                next = neighbor;
                break;
            }
        }
        current = next;
    }

    for (int i = 0; i < points_to_remove->size; i++) {
        Connection_point *rm_point = array_get(points_to_remove, i);

        for (int j = 0; j < con->points->size; j++) {
            Connection_point *other = array_get(con->points, j);
            array_remove(other->neighbors, rm_point);
        }
        array_remove(con->points, rm_point);
        free_connection_point(rm_point);
    }

    free(points_to_remove);
}

Connection_point *create_connection_point(Connection *con, int x, int y, Pin *linked_pin) {
    Connection_point *point = malloc(sizeof(Connection_point));
    if (point == NULL) {
        return NULL;
    }

    point->x = x;
    point->y = y;
    point->parent_connection = con;
    point->neighbors = array_create(2);
    point->linked_to_pin = linked_pin;

    return point;
}

void free_connection_point(Connection_point *point) {
    assert(point != NULL);
    free(point->neighbors);
    free(point);
}

Connection_point *add_connection_point(Connection *con, int x, int y, Pin *linked_pin) {
    assert(con != NULL);
    assert(con->points != NULL);

    Connection_point *point = create_connection_point(con, x, y, linked_pin);
    point->parent_connection = con;
    point->neighbors = array_create(2);
    array_add(con->points, point);
    return point;
}

void finalize_connection(Connection *con, Pin *pin) {
    assert(con != NULL);
    assert(pin != NULL);

    add_pin(con, pin);

    propagate_state(con);
    update_connection_geometry(con);
}

SDL_Point get_pin_center(Pin *pin) {
    return (SDL_Point) {
        .x = pin->parent_node->rect.x + pin->x + (PIN_SIZE / 2),
        .y = pin->parent_node->rect.y + pin->y + (PIN_SIZE / 2)
    };
}

void update_connection_geometry(Connection *con) {
    assert(con != NULL);
    assert(con->points != NULL);

    for (int i = 0; i < con->points->size; i++) {
        Connection_point *point = array_get(con->points, i);
        if (point->neighbors->size == 1 && point->linked_to_pin != NULL) {
            SDL_Point pin_center = get_pin_center(point->linked_to_pin);
            point->x = pin_center.x;
            point->y = pin_center.y;
        }
    }
}

void propagate_state(Connection *con) {
    assert(con != NULL);

    con->state = 0;
    for (int i = 0; i < con->output_pins->size; i++) {
        Pin *pin = array_get(con->output_pins, i);
        if (pin->state) {
            con->state = 1;
            break;
        }
    }

    for (int i = 0; i < con->input_pins->size; i++) {
        Pin *pin = array_get(con->input_pins, i);
        pin->state = con->state;
    }
}

void free_connection(Connection *con) {
    assert(con != NULL);
    // printf("‼️ free_connection - connection %p\n", (void*)con);

    if (con->points != NULL) {
        array_free(con->points);
        con->points = NULL;
    }

    free(con->input_pins);
    free(con->output_pins);
    free(con);
    con = NULL;
}

DynamicArray* find_fully_selected_connections(DynamicArray *selected_connection_points) {
    assert(selected_connection_points != NULL);

    DynamicArray *parent_connections = array_create(16);
    DynamicArray *fully_selected = array_create(16);
    HashMap *selection_counts = hashmap_create(8);

    for (int i = 0; i < selected_connection_points->size; i++) {
        Connection_point *point = array_get(selected_connection_points, i);
        if (!array_contains(parent_connections, point->parent_connection)) { array_add(parent_connections, point->parent_connection); }
    }

    for (int i = 0; i < selected_connection_points->size; i++) {
        Connection_point *point = (Connection_point *)array_get(selected_connection_points, i);
        Connection *parent = point->parent_connection;

        int *count_ptr = (int *)hashmap_get(selection_counts, parent);
        int count = count_ptr ? *count_ptr : 0;

        int *new_count = malloc(sizeof(int));
        *new_count = count + 1;
        hashmap_put(selection_counts, parent, new_count);
    }

    for (int i = 0; i < parent_connections->size; i++) {
        Connection *conn = (Connection *)array_get(parent_connections, i);
        int *count_ptr = (int *)hashmap_get(selection_counts, conn);
        int selected_count = count_ptr ? *count_ptr : 0;

        if (selected_count == conn->points->size && conn->points->size > 0) {
            array_add(fully_selected, conn);
        }
    }

    for (int i = 0; i < selection_counts->capacity; i++) {
        if (selection_counts->entries[i].occupied) {
            free(selection_counts->entries[i].value);
        }
    }
    hashmap_free(selection_counts);
    return fully_selected;
}

Connection* copy_connection(Connection *original_conn, DynamicArray *source_nodes, DynamicArray *target_nodes, float offset_x, float offset_y) {
    assert(original_conn != NULL);
    assert(source_nodes != NULL);
    assert(target_nodes != NULL);

    Connection *new_con = malloc(sizeof(Connection));
    if (new_con == NULL) {
        return NULL;
    }

    new_con->state = 0;
    new_con->input_pins = array_create(2);
    new_con->output_pins = array_create(2);
    new_con->points = array_create(2);

    for (int j = 0; j < original_conn->input_pins->size; j++) {
        Pin *original_pin = array_get(original_conn->input_pins, j);
        Pin *new_pin = find_corresponding_pin(original_pin, source_nodes, target_nodes);
        if (new_pin != NULL) {
            array_add(new_con->input_pins, new_pin);
            array_add(new_pin->connected_connections, new_con);
        }
    }

    for (int j = 0; j < original_conn->output_pins->size; j++) {
        Pin *original_pin = array_get(original_conn->output_pins, j);
        Pin *new_pin = find_corresponding_pin(original_pin, source_nodes, target_nodes);
        if (new_pin != NULL) {
            array_add(new_con->output_pins, new_pin);
            array_add(new_pin->connected_connections, new_con);
        }
    }

    for (int j = 0; j < original_conn->points->size; j++) {
        Connection_point *original_point = array_get(original_conn->points, j);
        Pin *linked_pin = NULL;
        if (original_point->linked_to_pin != NULL) {
            linked_pin = find_corresponding_pin(original_point->linked_to_pin, source_nodes, target_nodes);
        }
        add_connection_point(new_con,  original_point->x + offset_x,  original_point->y + offset_y, linked_pin);
    }

    for (int j = 0; j < original_conn->points->size; j++) {
        Connection_point *original_point = array_get(original_conn->points, j);
        Connection_point *new_point = array_get(new_con->points, j);

        for (int k = 0; k < original_point->neighbors->size; k++) {
            Connection_point *original_neighbor = array_get(original_point->neighbors, k);

            int neighbor_index = -1;
            for (int l = 0; l < original_conn->points->size; l++) {
                if (array_get(original_conn->points, l) == original_neighbor) {
                    neighbor_index = l;
                    break;
                }
            }

            if (neighbor_index >= 0 && neighbor_index < new_con->points->size) {
                Connection_point *new_neighbor = array_get(new_con->points, neighbor_index);
                array_add(new_point->neighbors, new_neighbor);
            }
        }
    }

    return new_con;
}

void print_connection_graph(Connection *con) {
    assert(con != NULL);

    if (!con) {
        printf("Invalid connection (NULL pointer)\n");
        return;
    }

    printf("=== Connection Graph Visualization ===\n");
    printf("Connection (%p):\n", (void *)con);

    printf("\n--- Adjacency Matrix ---\n");
    printf("    ");
    for (int i = 0; i < con->points->size; i++) {
        printf("P[%d] ", i);
    }
    printf("\n");

    for (int i = 0; i < con->points->size; i++) {
        Connection_point *point = array_get(con->points, i);
        printf("P[%d] ", i);

        for (int j = 0; j < con->points->size; j++) {
            Connection_point *other_point = array_get(con->points, j);
            if (array_contains(point->neighbors, other_point)) {
                printf("  1  ");
            } else {
                printf("  0  ");
            }
        }
        printf("\n");
    }
}

void print_connection(Connection *con) {
    if (!con) {
        printf("Invalid connection (NULL pointer)\n");
        return;
    }

    printf("Connection (%p):\n", (void *)con);

    printf("  Input Pins:\n");
    for (int i = 0; i < con->input_pins->size; i++) {
        Pin *pin = (Pin *)array_get(con->input_pins, i);
        Node *node = pin->parent_node;
        printf("    Pin %d: Node: %s (%p) | Pin (%p) State: %d | ID: %d | Pos: (%d, %d)\n",
               i,
               node ? node->name : "NULL",
               (void *)node,
               (void *)pin,
               pin->state,
               pin->id,
               pin->x, pin->y);
    }

    printf("  Output Pins:\n");
    for (int i = 0; i < con->output_pins->size; i++) {
        Pin *pin = (Pin *)array_get(con->output_pins, i);
        Node *node = pin->parent_node;
        printf("    Pin %d: Node: %s (%p) | Pin (%p) State: %d | ID: %d | Pos: (%d, %d)\n",
               i,
               node ? node->name : "NULL",
               (void *)node,
               (void *)pin,
               pin->state,
               pin->id,
               pin->x, pin->y);
    }

    printf("  Cable State: %d\n", con->state);

    printf("  Connection_point:\n");
    for (int i = 0; i < con->points->size; i++) {
        Connection_point *point = (Connection_point *)array_get(con->points, i);
        printf("    Connection_point (%p) %d (x: %d, y: %d) | neighbors: %d | parent_connection: %p | linked_to_pin: %p\n", (void *)point, i, point->x, point->y, point->neighbors->size, (void *)point->parent_connection, (void *)point->linked_to_pin);
    }

    printf("--------------------------\n");
}
