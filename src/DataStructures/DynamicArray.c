#include "DynamicArray.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

DynamicArray *array_create(int init_capacity) {
    assert(init_capacity > 0);
    DynamicArray *new_array = malloc(sizeof(DynamicArray));
    if (new_array == NULL) {
        return NULL;
    }

    new_array->size = 0;
    new_array->capacity = init_capacity;
    new_array->data = malloc(sizeof(void*) * init_capacity);
    if (new_array->data == NULL) {
        free(new_array);
        return NULL;
    }

    return new_array;
}

DynamicArray *array_create_empty_with_size(int size) {
    DynamicArray * arr = array_create(1);
    if (arr == NULL) return NULL;

    for (int i = 0; i < size; i++) {
        int* val = malloc(sizeof(int));
        if (val == NULL) {
            array_free(arr);
            return NULL;
        }
        *val = -1;
        array_add(arr, val);
    }

    return arr;
}

void array_free(DynamicArray *arr) {
    if (arr == NULL) return;

    for (int i = 0; i < arr->size; i++) {
        if (arr->data[i] != NULL) {
            free(arr->data[i]);
            arr->data[i] = NULL;
        }
    }
    free(arr->data);
    free(arr);
}

void array_clear(DynamicArray *arr) {
    if (arr == NULL) return;

    arr->size = 0;
}

void array_add(DynamicArray *arr, void *element) {
    if (arr->size == arr->capacity) {
        arr->capacity = arr->capacity == 0 ? 4 : arr->capacity * 2;
        arr->data = realloc(arr->data, arr->capacity * sizeof(void*));
    }

    arr->data[arr->size++] = element;
}

void *array_get(DynamicArray *arr, int index) {
    assert(arr != NULL);
    assert(index >= 0 && index < arr->size);
    return arr->data[index];
}

void array_remove(DynamicArray *arr, void *element) {
    assert(arr != NULL);
    assert(element != NULL);
    for (int i = 0; i < arr->size; i++) {
        if (arr->data[i] == element) {
            array_remove_at(arr, i);
            return;
        }
    }
}

void array_remove_at(DynamicArray *arr, int index) {
    assert(arr != NULL);
    assert(index < arr->size);

    int last_index = arr->size - 1;

    if (index != last_index) {
        arr->data[index] = arr->data[last_index];
    }

    arr->size--;

    if (arr->size > 0 && arr->size <= arr->capacity / 4) {
        int new_capacity = arr->capacity / 2;
        void **new_memory = realloc(arr->data, new_capacity * sizeof(void *));
        if (new_memory == NULL) {
            printf("ERROR - array_remove_at: resize error\n");
            return;
        }
        arr->data = new_memory;
        arr->capacity = new_capacity;
    }
}

void array_remove_last(DynamicArray *arr) {
    assert(arr != NULL);
    assert(arr->size > 0);
    array_remove_at(arr, arr->size - 1);
}

DynamicArray *flat_copy(DynamicArray *from_arr) {
    assert(from_arr != NULL);

    DynamicArray *to_arr = array_create(from_arr->capacity);
    for (int i = 0; i < from_arr->size; i++) {
        array_add(to_arr, from_arr->data[i]);
    }
    return to_arr;
}

void array_move_all(DynamicArray *to_arr, DynamicArray *from_arr) {
    assert(to_arr != NULL);
    assert(from_arr != NULL);

    for (int i = 0; i < from_arr->size; i++) {
        array_add(to_arr, from_arr->data[i]);
    }
    from_arr->size = 0;
}

int array_contains(DynamicArray *array, void *element) {
    if (array == NULL || element == NULL) return 0;

    for (int i = 0; i < array->size; i++) {
        if (array->data[i] == element) {
            return 1;
        }
    }
    return 0;
}

int array_get_index(DynamicArray *array, void *element) {
    if (array == NULL || element == NULL) return 0;

    for (int i = 0; i < array->size; i++) {
        if (array->data[i] == element) {
            return i;
        }
    }
    return -1;
}

void array_print_stats(DynamicArray *arr, const char *label) {
    size_t element_size = sizeof(void*);
    size_t elements_in_array = arr->size;
    size_t used_storage = elements_in_array * element_size;
    size_t total_storage = arr->capacity * element_size;

    printf("📊 Array stats for %s:\n", label);
    printf("  → Elements in array: %zu\n", elements_in_array);
    printf("  → Element size: %zu bytes\n", element_size);
    printf("  → Storage used: %zu bytes\n", used_storage);
    printf("  → Total storage reserved: %zu bytes\n", total_storage);
    printf("-----------------------------------\n");
}
