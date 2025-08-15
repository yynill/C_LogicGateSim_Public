#ifndef Button_H
#define Button_H
#pragma once

#include "main.h"

typedef struct Button
{
    Float_Rect rect;
    char *name;
    void *function_data;
    void (*on_press)(void *function_data);
} Button;

Button *create_button(Float_Rect rect, char *name, void *function_data, void (*on_press)(void*));

#endif // Button_H
