#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#pragma once

#include "main.h"
typedef struct cJSON cJSON;
typedef struct Connection Connection;
typedef struct Node Node;
typedef struct DynamicArray DynamicArray;

cJSON *connection_to_json(Connection *con);
cJSON *node_to_json(Node *node);

void save_graph_to_json();
void load_graph_from_json(void *function_data);

// Helper functions for JSON parsing
Operation* get_operation_from_string(const char *op_name);
Node* json_to_node(cJSON *node_json);
Connection* json_to_connection(DynamicArray *node_layer, cJSON *connection_json);

#endif // FILEHANDLER_H
