

#include "popup_state.h"

PopupState *init_popupstate(char *title, void (*on_confirm)(void)) {
    PopupState *pop_state = malloc(sizeof(PopupState));
    if (!pop_state) return NULL;

    pop_state->title = title;
    pop_state->on_confirm = on_confirm;
    pop_state->on_cancel = handle_popup_cancel;

    int popup_w = 600;
    int popup_h = 140;

    int popup_x = (WINDOW_WIDTH - popup_w) / 2;
    int popup_y = (WINDOW_HEIGHT - popup_h) / 2;

    pop_state->rect.x = popup_x,
    pop_state->rect.y = popup_y,
    pop_state->rect.w = popup_w;
    pop_state->rect.h = popup_h;

    pop_state->name_input.rect.x = popup_x + PADDING;
    pop_state->name_input.rect.y = popup_y + (3 * PADDING);
    pop_state->name_input.rect.w = popup_w - (2 * PADDING);
    pop_state->name_input.rect.h = 30;
    pop_state->name_input.text[0] = '\0';

    pop_state->buttons = array_create(16);
    assert(pop_state->buttons != NULL);

    int button_w = 40;
    int button_h = 20;

    Float_Rect enter_rect = {
        pop_state->rect.x + pop_state->rect.w - button_w - PADDING,
        pop_state->rect.y + pop_state->rect.h - button_h - PADDING,
        button_w,
        button_h
    };

    pop_state->enter_button = create_button(enter_rect, "enter", nullGate, handle_enter);

    Float_Rect esc_rect = {
        enter_rect.x - button_w - PADDING,
        enter_rect.y,
        button_w,
        button_h
    };
    pop_state->esc_button = create_button(esc_rect, "esc", nullGate, handle_escape);

    update_popup_layout(pop_state);
    return pop_state;
}

void update_popup_layout(PopupState *popstate) {
    if (!popstate || !popstate->buttons || popstate->buttons->size == 0) return;

    int button_count = popstate->buttons->size;

    int buttons_per_row = 3;
    int button_w = (popstate->rect.w - ((2 + buttons_per_row - 1) * PADDING)) / buttons_per_row;
    int button_h = 30;
    int rows_needed = (button_count + buttons_per_row - 1) / buttons_per_row;

    int buttons_height = rows_needed * button_h + (rows_needed - 1) * PADDING;

    int popup_w = popstate->rect.w;

    int title_area_height = 3 * PADDING;
    int name_input_height = 30 + PADDING;
    int buttons_area_height = buttons_height + PADDING;
    int control_buttons_height = popstate->enter_button->rect.h + 2 * PADDING;

    int popup_h = title_area_height + name_input_height + buttons_area_height + control_buttons_height;

    int popup_x = (WINDOW_WIDTH - popup_w) / 2;
    int popup_y = (WINDOW_HEIGHT - popup_h) / 2;

    popstate->rect.y = popup_y;
    popstate->rect.h = popup_h;

    int start_y = popup_y + title_area_height + name_input_height + PADDING;
    int current_row = 0;
    int current_col = 0;

    for (int i = 0; i < button_count; i++) {
        Button *button = (Button *)array_get(popstate->buttons, i);
        if (!button) continue;

        int button_x = popup_x + PADDING + current_col * (button_w + PADDING);
        int button_y = start_y + current_row * (button_h + PADDING);

        button->rect.x = button_x;
        button->rect.y = button_y;
        button->rect.w = button_w;
        button->rect.h = button_h;

        current_col++;
        if (current_col >= buttons_per_row) {
            current_col = 0;
            current_row++;
        }
    }

    int control_button_h = popstate->enter_button->rect.h;
    int control_y = popup_y + popup_h - control_button_h - PADDING;

    popstate->enter_button->rect.y = control_y;
    popstate->esc_button->rect.y = control_y;
    popstate->name_input.rect.y = popup_y + title_area_height;
}

void handle_popup_cancel() {
    if (sim_state->popup_state) {
        sim_state->popup_state = clear_popup(sim_state->popup_state);
    }
}

PopupState *clear_popup(PopupState *popstate) {
    if (!popstate) return NULL;

    if (popstate->buttons) {
        array_free(popstate->buttons);
    }

    free(popstate);
    return NULL;
}

void add_popup_button(PopupState *popstate, Button *button) {
    if (!popstate || !button) return;

    array_add(popstate->buttons, button);
    update_popup_layout(popstate);
}

int try_handle_popup() {
    Button *clicked_button = find_button_at_position(sim_state->popup_state->buttons, sim_state->mouse_x, sim_state->mouse_y);
    if (clicked_button) {
        clicked_button->on_press(clicked_button->function_data);
        return 1;
    }
    return 0;
}
