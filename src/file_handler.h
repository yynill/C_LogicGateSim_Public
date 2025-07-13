#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#pragma once

#include <assert.h>
#include "cJSON/cJSON.h"
#include "DataStructures/DynamicArray.h"
#include "node.h"
#include "connection.h"
#include "operation.h"
#include "simulation.h"
#include <string.h>
#include <stdint.h>
#include <SDL2/SDL.h>

cJSON *connection_to_json(Connection *con);
cJSON *node_to_json(Node *node);

void save_graph_to_json();
void load_graph_from_json(void *function_data);

// Helper functions for JSON parsing
Operation* get_operation_from_string(const char *op_name);
Node* json_to_node(cJSON *node_json);
Connection* json_to_connection(cJSON *connection_json);

#endif // FILEHANDLER_H
