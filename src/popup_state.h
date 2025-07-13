#ifndef POPUPSTATE_H
#define POPUPSTATE_H
#pragma once

#include <assert.h>
#include <SDL2/SDL.h>
#include <string.h>

#include "renderer.h"
#include "button.h"
#include "DataStructures/DynamicArray.h"
#include "simulation.h"

typedef struct Button Button;

typedef struct {
    SDL_Rect rect;
    char text[256];
} TextInput;

typedef struct PopupState {
    const char* title;
    SDL_Rect rect;

    TextInput name_input;
    DynamicArray *buttons;

    Button *enter_button;
    Button *esc_button;

    void (*on_confirm)(void);
    void (*on_cancel)(void);
} PopupState;

PopupState *init_popupstate(const char *title, void (*on_confirm)(void));
PopupState *clear_popup(PopupState *popstate);
void handle_popup_cancel();
int try_handle_popup();
void add_popup_button(PopupState *popstate, Button *button);
void update_popup_layout(PopupState *popstate);

#endif // POPUPSTATE_H
