#include "file_handler.h"

void save_graph_to_json() {
    DynamicArray *nodes = sim_state->nodes;
    DynamicArray *connections = sim_state->connections;

    char filename[256];
    FILE *file;
    cJSON *tree = cJSON_CreateObject();
    cJSON *nodes_json = cJSON_CreateArray();
    cJSON *connections_json = cJSON_CreateArray();

    for (int i = 0; i < nodes->size; i++) {
        Node *node = array_get(nodes, i);
        cJSON *element = node_to_json(node);
        cJSON_AddItemToArray(nodes_json, element);
    }
    cJSON_AddItemToObject(tree, "nodes", nodes_json);

    for (int i = 0; i < connections->size; i++) {
        Connection *con = array_get(connections, i);
        cJSON *con_json = connection_to_json(con);
        cJSON_AddItemToArray(connections_json, con_json);
    }
    cJSON_AddItemToObject(tree, "connections", connections_json);

    snprintf(filename, sizeof(filename), "circuit_files/%s.json", sim_state->popup_state->name_input.text);

    char *string = cJSON_Print(tree);
    if (string == NULL) {
        fprintf(stderr, "Failed to print json tree.\n");
    }

    file = fopen(filename, "w");
    assert(file);
    fprintf(file, "%s", string);
    fclose(file);

    cJSON_Delete(tree);
    free(string);
}

void load_graph_from_json(void *function_data) {
    char filename[256];
    if (function_data != NULL) {
        snprintf(filename, sizeof(filename), "circuit_files/%s", (char*)function_data);
    } else if (sim_state->popup_state != NULL) {
        snprintf(filename, sizeof(filename), "circuit_files/%s.json", sim_state->popup_state->name_input.text);
    } else {
        fprintf(stderr, "No filename provided for loading\n");
        return;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *json_string = malloc(file_size + 1);
    if (!json_string) {
        fclose(file);
        return;
    }

    size_t bytes_read = fread(json_string, 1, file_size, file);
    json_string[bytes_read] = '\0';
    fclose(file);

    cJSON *root = cJSON_Parse(json_string);
    free(json_string);

    if (!root) {
        fprintf(stderr, "Failed to parse JSON\n");
        return;
    }

    int max_pin_id = -1;
    cJSON *nodes_array = cJSON_GetObjectItem(root, "nodes");
    if (nodes_array && cJSON_IsArray(nodes_array)) {
        for (int i = 0; i < cJSON_GetArraySize(nodes_array); i++) {
            cJSON *node_json = cJSON_GetArrayItem(nodes_array, i);
            cJSON *inputs_json = cJSON_GetObjectItem(node_json, "inputs");
            cJSON *outputs_json = cJSON_GetObjectItem(node_json, "outputs");

            if (inputs_json && cJSON_IsArray(inputs_json)) {
                for (int j = 0; j < cJSON_GetArraySize(inputs_json); j++) {
                    cJSON *pin_id_json = cJSON_GetArrayItem(inputs_json, j);
                    int pin_id = (int)cJSON_GetNumberValue(pin_id_json);
                    if (pin_id > max_pin_id) max_pin_id = pin_id;
                }
            }

            if (outputs_json && cJSON_IsArray(outputs_json)) {
                for (int j = 0; j < cJSON_GetArraySize(outputs_json); j++) {
                    cJSON *pin_id_json = cJSON_GetArrayItem(outputs_json, j);
                    int pin_id = (int)cJSON_GetNumberValue(pin_id_json);
                    if (pin_id > max_pin_id) max_pin_id = pin_id;
                }
            }
        }
    }

    if (max_pin_id >= 0) {
        next_pin_id = max_pin_id + 1;
    }

    if (nodes_array && cJSON_IsArray(nodes_array)) {
        for (int i = 0; i < cJSON_GetArraySize(nodes_array); i++) {
            cJSON *node_json = cJSON_GetArrayItem(nodes_array, i);
            Node *node = json_to_node(node_json);
            if (node) {
                array_add(sim_state->nodes, node);
                if (!queue_contains(sim_state->node_queue, node)) {
                    enqueue(sim_state->node_queue, node);
                }
            }
        }
    }

    cJSON *connections_array = cJSON_GetObjectItem(root, "connections");
    if (connections_array && cJSON_IsArray(connections_array)) {
        for (int i = 0; i < cJSON_GetArraySize(connections_array); i++) {
            cJSON *connection_json = cJSON_GetArrayItem(connections_array, i);
            Connection *connection = json_to_connection(sim_state->nodes, connection_json);
            if (connection) {
                array_add(sim_state->connections, connection);
                propagate_state(connection);
                update_connection_geometry(connection);
            }
        }
    }

    cJSON_Delete(root);
}

Operation* get_operation_from_string(const char *op_name) {
    if (strcmp(op_name, "nullGate") == 0) return &nullGate;
    if (strcmp(op_name, "noteNode") == 0) return &noteNode;
    if (strcmp(op_name, "lightNode") == 0) return &lightNode;
    if (strcmp(op_name, "switchNode") == 0) return &switchNode;
    if (strcmp(op_name, "notGate") == 0) return &notGate;
    if (strcmp(op_name, "andGate") == 0) return &andGate;
    if (strcmp(op_name, "nandGate") == 0) return &nandGate;
    if (strcmp(op_name, "orGate") == 0) return &orGate;
    if (strcmp(op_name, "norGate") == 0) return &norGate;
    if (strcmp(op_name, "xorGate") == 0) return &xorGate;
    if (strcmp(op_name, "xnorGate") == 0) return &xnorGate;
    return &nullGate;
}

Node* json_to_node(cJSON *node_json) {
    if (!node_json || !cJSON_IsObject(node_json)) {
        return NULL;
    }

    Node *node;

    cJSON *name_json = cJSON_GetObjectItem(node_json, "name");
    cJSON *pos_json = cJSON_GetObjectItem(node_json, "pos");
    cJSON *x_json = cJSON_GetObjectItem(pos_json, "x");
    cJSON *y_json = cJSON_GetObjectItem(pos_json, "y");
    cJSON *inputs_json = cJSON_GetObjectItem(node_json, "inputs");
    cJSON *outputs_json = cJSON_GetObjectItem(node_json, "outputs");
    cJSON *operation_json = cJSON_GetObjectItem(node_json, "operation");
    cJSON *sub_nodes_json = cJSON_GetObjectItem(node_json, "sub_nodes");
    cJSON *sub_connections_json = cJSON_GetObjectItem(node_json, "sub_connections");

    const char *name = cJSON_GetStringValue(name_json);
    SDL_Point pos = {
        .x = cJSON_GetNumberValue(x_json),
        .y = cJSON_GetNumberValue(y_json)
    };
    const char *op_name = cJSON_GetStringValue(operation_json);
    Operation *operation = get_operation_from_string(op_name);

    DynamicArray *inputs = array_create(1);
    if (inputs_json && cJSON_IsArray(inputs_json)) {
        for (int i = 0; i < cJSON_GetArraySize(inputs_json); i++) {
            cJSON *pin_id_json = cJSON_GetArrayItem(inputs_json, i);
            int *pin_id = malloc(sizeof(int));
            *pin_id = (int)cJSON_GetNumberValue(pin_id_json);
            array_add(inputs, pin_id);
        }
    }
    DynamicArray *outputs = array_create(1);
    if (outputs_json && cJSON_IsArray(outputs_json)) {
        for (int i = 0; i < cJSON_GetArraySize(outputs_json); i++) {
            cJSON *pin_id_json = cJSON_GetArrayItem(outputs_json, i);
            int *pin_id = malloc(sizeof(int));
            *pin_id = (int)cJSON_GetNumberValue(pin_id_json);
            array_add(outputs, pin_id);
        }
    }

    int sub_nodes_size = cJSON_GetArraySize(sub_nodes_json);
    if (sub_nodes_size > 0) {
        DynamicArray *sub_nodes = array_create(8);
        DynamicArray *sub_connections = array_create(8);

        for (int i = 0; i < sub_nodes_size; i++) {
            cJSON* sub_node_json = cJSON_GetArrayItem(sub_nodes_json, i);
            Node *sub_node = json_to_node(sub_node_json);
            if (sub_node != NULL) array_add(sub_nodes, sub_node);
        }

        int sub_connection_size = cJSON_GetArraySize(sub_connections_json);
        for (int i = 0; i < sub_connection_size; i++) {
            cJSON *sub_con_json = cJSON_GetArrayItem(sub_connections_json, i);
            Connection *sub_con = json_to_connection(sub_nodes, sub_con_json);
            if (sub_con) {
                array_add(sub_connections, sub_con);
                propagate_state(sub_con);
                update_connection_geometry(sub_con);
            }
        }

        node = create_group_node(&pos, inputs->size, outputs->size, name, sub_nodes, sub_connections);

        if (inputs_json && cJSON_IsArray(inputs_json)) {
            for (int i = 0; i < cJSON_GetArraySize(inputs_json) && i < node->inputs->size; i++) {
                cJSON *pin_id_json = cJSON_GetArrayItem(inputs_json, i);
                Pin *pin = array_get(node->inputs, i);
                pin->id = (int)cJSON_GetNumberValue(pin_id_json);
            }
        }

        if (outputs_json && cJSON_IsArray(outputs_json)) {
            for (int i = 0; i < cJSON_GetArraySize(outputs_json) && i < node->outputs->size; i++) {
                cJSON *pin_id_json = cJSON_GetArrayItem(outputs_json, i);
                Pin *pin = array_get(node->outputs, i);
                pin->id = (int)cJSON_GetNumberValue(pin_id_json);
            }
        }

        array_free(sub_nodes);
        array_free(sub_connections);
    }
    else {
        node = create_node(inputs->size, outputs->size, operation, &pos, name);

        if (inputs_json && cJSON_IsArray(inputs_json)) {
            for (int i = 0; i < cJSON_GetArraySize(inputs_json) && i < node->inputs->size; i++) {
                cJSON *pin_id_json = cJSON_GetArrayItem(inputs_json, i);
                Pin *pin = array_get(node->inputs, i);
                pin->id = (int)cJSON_GetNumberValue(pin_id_json);
            }
        }

        if (outputs_json && cJSON_IsArray(outputs_json)) {
            for (int i = 0; i < cJSON_GetArraySize(outputs_json) && i < node->outputs->size; i++) {
                cJSON *pin_id_json = cJSON_GetArrayItem(outputs_json, i);
                Pin *pin = array_get(node->outputs, i);
                pin->id = (int)cJSON_GetNumberValue(pin_id_json);
            }
        }
    }

    array_free(inputs);
    array_free(outputs);

    return node;
}

Connection* json_to_connection(DynamicArray *node_layer, cJSON *connection_json) {
    if (!connection_json || !cJSON_IsObject(connection_json)) {
        return NULL;
    }

    Connection *connection = malloc(sizeof(Connection));
    connection->input_pins = array_create(1);
    connection->output_pins = array_create(1);
    connection->points = array_create(4);
    connection->state = 0;

    cJSON *input_pins_json = cJSON_GetObjectItem(connection_json, "input_pins");
    cJSON *output_pins_json = cJSON_GetObjectItem(connection_json, "output_pins");
    cJSON *points_json = cJSON_GetObjectItem(connection_json, "points");


    int input_pins_size = cJSON_GetArraySize(input_pins_json);
    for (int i = 0; i < input_pins_size; i++) {
        cJSON *pin_id_json = cJSON_GetArrayItem(input_pins_json, i);
        Pin *pin = find_pin_by_id(node_layer, pin_id_json->valueint);
        if (pin != NULL) {
            array_add(connection->input_pins, pin);
            array_add(pin->connected_connections, connection);
        }
    }

    int output_pins_size = cJSON_GetArraySize(output_pins_json);
    for (int i = 0; i < output_pins_size; i++) {
        cJSON *pin_id_json = cJSON_GetArrayItem(output_pins_json, i);
        Pin *pin = find_pin_by_id(node_layer, pin_id_json->valueint);
        if (pin != NULL) {
            array_add(connection->output_pins, pin);
            array_add(pin->connected_connections, connection);
        }
    }

    int points_size = cJSON_GetArraySize(points_json);
    for (int i = 0; i < points_size; i++) {
        cJSON * point_json= cJSON_GetArrayItem(points_json, i);
        cJSON *x_json = cJSON_GetObjectItem(point_json, "x");
        cJSON *y_json = cJSON_GetObjectItem(point_json, "y");
        cJSON *linked_pin_id_json = cJSON_GetObjectItem(point_json, "linked_pin_id");
        Pin *linked_pin = NULL;
        if (linked_pin_id_json->valueint != -1) {
            linked_pin = find_pin_by_id(node_layer, linked_pin_id_json->valueint);
        }
        add_connection_point(connection, x_json->valueint, y_json->valueint, linked_pin);
    }

    for (int i = 0; i < points_size; i++) {
        cJSON * point_json= cJSON_GetArrayItem(points_json, i);
        cJSON *neighbors_json = cJSON_GetObjectItem(point_json, "neighbors");
        int neighbors_size = cJSON_GetArraySize(neighbors_json);
        for (int j = 0; j < neighbors_size; j++) {
            Connection_point *point = array_get(connection->points, i);
            cJSON *neighbor_json = cJSON_GetArrayItem(neighbors_json, j);
            Connection_point *neighbour = array_get(connection->points, neighbor_json->valueint);
            if (point < neighbour) {
                add_connection_link(point, neighbour);
            }
        }
    }

    return connection;
}

cJSON *connection_to_json(Connection *con) {
    if (con == NULL) return NULL;
    cJSON *connection_json = cJSON_CreateObject();

    if (con->input_pins && con->input_pins->size > 0) {
        cJSON *input_pins_json = cJSON_CreateArray();
        for (int i = 0; i < con->input_pins->size; i++) {
            Pin *pin = array_get(con->input_pins, i);
            cJSON_AddItemToArray(input_pins_json, cJSON_CreateNumber(pin->id));
        }
        cJSON_AddItemToObject(connection_json, "input_pins", input_pins_json);
    }

    if (con->output_pins && con->output_pins->size > 0) {
        cJSON *output_pins_json = cJSON_CreateArray();
        for (int i = 0; i < con->output_pins->size; i++) {
            Pin *pin = array_get(con->output_pins, i);
            cJSON_AddItemToArray(output_pins_json, cJSON_CreateNumber(pin->id));
        }
        cJSON_AddItemToObject(connection_json, "output_pins", output_pins_json);
    }

    if (con->points) {
        cJSON *points_json = cJSON_CreateArray();
        for (int i = 0; i < con->points->size; i++) {
            Connection_point *point = array_get(con->points, i);
            cJSON *point_json = cJSON_CreateObject();
            cJSON_AddNumberToObject(point_json, "x", point->pos.x);
            cJSON_AddNumberToObject(point_json, "y", point->pos.y);

            if (point->linked_to_pin) {
                cJSON_AddNumberToObject(point_json, "linked_pin_id", point->linked_to_pin->id);
            } else {
                cJSON_AddNumberToObject(point_json, "linked_pin_id", -1);
            }

            if (point->neighbors && point->neighbors->size > 0) {
                cJSON *neighbors_json = cJSON_CreateArray();
                for (int j = 0; j < point->neighbors->size; j++) {
                    Connection_point *neighbor = array_get(point->neighbors, j);
                    for (int k = 0; k < con->points->size; k++) {
                        if (array_get(con->points, k) == neighbor) {
                            cJSON_AddItemToArray(neighbors_json, cJSON_CreateNumber(k));
                            break;
                        }
                    }
                }
                cJSON_AddItemToObject(point_json, "neighbors", neighbors_json);
            }

            cJSON_AddItemToArray(points_json, point_json);
        }
        cJSON_AddItemToObject(connection_json, "points", points_json);
    }

    return connection_json;
}


cJSON *node_to_json(Node *node) {
    if (node == NULL) return NULL;

    cJSON *node_json = cJSON_CreateObject();

    cJSON_AddStringToObject(node_json, "name", node->name);

    cJSON *pos_json = cJSON_CreateObject();
    cJSON_AddNumberToObject(pos_json, "x", node->rect.x);
    cJSON_AddNumberToObject(pos_json, "y", node->rect.y);
    cJSON_AddItemToObject(node_json, "pos", pos_json);

    cJSON *inputs_json = cJSON_CreateArray();
    for (int i = 0; i < node->inputs->size; i++) {
        Pin *pin = array_get(node->inputs, i);
        cJSON_AddItemToArray(inputs_json, cJSON_CreateNumber(pin->id));
    }
    cJSON_AddItemToObject(node_json, "inputs", inputs_json);

    cJSON *outputs_json = cJSON_CreateArray();
    for (int i = 0; i < node->outputs->size; i++) {
        Pin *pin = array_get(node->outputs, i);
        cJSON_AddItemToArray(outputs_json, cJSON_CreateNumber(pin->id));
    }
    cJSON_AddItemToObject(node_json, "outputs", outputs_json);

    const char *op_name = "unknown";
    if (node->operation == &nullGate) op_name = "nullGate";
    else if (node->operation == &noteNode) op_name = "noteNode";
    else if (node->operation == &lightNode) op_name = "lightNode";
    else if (node->operation == &switchNode) op_name = "switchNode";
    else if (node->operation == &notGate) op_name = "notGate";
    else if (node->operation == &andGate) op_name = "andGate";
    else if (node->operation == &nandGate) op_name = "nandGate";
    else if (node->operation == &orGate) op_name = "orGate";
    else if (node->operation == &norGate) op_name = "norGate";
    else if (node->operation == &xorGate) op_name = "xorGate";
    else if (node->operation == &xnorGate) op_name = "xnorGate";
    cJSON_AddItemToObject(node_json, "operation", cJSON_CreateString(op_name));

    cJSON *sub_nodes_json = cJSON_CreateArray();
    if (node->sub_nodes != NULL) {
        for (int i = 0; i < node->sub_nodes->size; i++) {
            Node *sub_node = array_get(node->sub_nodes, i);
            cJSON *sub_node_json = node_to_json(sub_node);
            if (sub_node_json != NULL) {
                cJSON_AddItemToArray(sub_nodes_json, sub_node_json);
            }
        }
    }
    cJSON_AddItemToObject(node_json, "sub_nodes", sub_nodes_json);

    cJSON *sub_conns_json = cJSON_CreateArray();
    if (node->sub_connections != NULL) {
        for (int i = 0; i < node->sub_connections->size; i++) {
            Connection *con = array_get(node->sub_connections, i);
            cJSON *con_json = connection_to_json(con);
            if (con_json != NULL) {
                cJSON_AddItemToArray(sub_conns_json, con_json);
            }
        }
    }
    cJSON_AddItemToObject(node_json, "sub_connections", sub_conns_json);
    return node_json;
}
