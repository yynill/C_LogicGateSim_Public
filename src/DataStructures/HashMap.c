#include "HashMap.h"

static int ptr_hash(void *ptr) {
    uintptr_t val = (uintptr_t)ptr;
    val = (val >> 4) | (val << (sizeof(uintptr_t) * 8 - 4));
    return val;
}

HashMap* hashmap_create(int capacity) {
    HashMap *map = malloc(sizeof(HashMap));
    map->capacity = capacity;
    map->size = 0;
    map->entries = calloc(capacity, sizeof(HashEntry));
    return map;
}

void hashmap_free(HashMap *map) {
    free(map->entries);
    free(map);
}

void hashmap_put(HashMap *map, void *key, void *value) {
    assert(map != NULL);

    int index = ptr_hash(key) % map->capacity;

    while (map->entries[index].occupied) {
        if (map->entries[index].key == key) {
            map->entries[index].value = value;
            return;
        }
        index = (index + 1) % map->capacity;
    }

    map->entries[index].key = key;
    map->entries[index].value = value;
    map->entries[index].occupied = 1;
    map->size++;
}

void* hashmap_get(HashMap *map, void *key) {
    assert(map != NULL);

    int index = ptr_hash(key) % map->capacity;

    while (map->entries[index].occupied) {
        if (map->entries[index].key == key) {
            return map->entries[index].value;
        }
        index = (index + 1) % map->capacity;
    }

    return NULL;
}
