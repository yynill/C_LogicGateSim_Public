#include "button.h"

Button *create_button(Float_Rect rect, char *name, void *function_data, void (*on_press)(void*)) {
    Button *btn = malloc(sizeof(Button));
    if (!btn) return NULL;
    btn->rect = rect;
    btn->name = strdup(name);
    btn->function_data = function_data;
    btn->on_press = on_press;
    return btn;
}

// todo: ? free button
