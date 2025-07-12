#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef struct HashEntry{
    void *key;
    void *value;
    int occupied;
} HashEntry;

typedef struct HashMap {
    HashEntry *entries;
    int capacity;
    int size;
} HashMap;

HashMap* hashmap_create(int capacity);
void hashmap_free(HashMap *map);

void hashmap_put(HashMap *map, void *key, void *value);
void* hashmap_get(HashMap *map, void *key);

#endif // HASHMAP_H
