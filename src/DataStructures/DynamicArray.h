#ifndef DynamicArray_H
#define DynamicArray_H
#pragma once

#include <stdlib.h>

typedef struct DynamicArray
{
    int size;
    int capacity;
    void **data;
} DynamicArray;

DynamicArray *array_create(int init_size);
void array_free(DynamicArray *arr);
void array_clear(DynamicArray *arr);
void array_add(DynamicArray *arr, void *element);
void *array_get(DynamicArray *arr, int index);
void array_remove(DynamicArray *arr, void *element);
void array_remove_at(DynamicArray *arr, int index);
void array_remove_last(DynamicArray *arr);
int array_contains(DynamicArray *array, void *element);
void array_print_stats(DynamicArray *arr, const char *label);
DynamicArray *flat_copy(DynamicArray *from_arr);
void array_move_all(DynamicArray *to_arr, DynamicArray *from_arr);

#endif
