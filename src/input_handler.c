#include "input_handler.h"

void handle_input(SDL_Event *event) {
    assert(event != NULL);

    if (sim_state->popup_state != NULL) SDL_StartTextInput();
    else SDL_StopTextInput();

    switch(event->type) {
        case SDL_TEXTINPUT:
            if (sim_state->popup_state != NULL) {
                strncat(sim_state->popup_state->name_input.text, event->text.text,
                        sizeof(sim_state->popup_state->name_input.text) - strlen(sim_state->popup_state->name_input.text) - 1);
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            handle_mouse_button_down(event);
            break;

        case SDL_MOUSEBUTTONUP:
            handle_mouse_button_up(event);
            break;

        case SDL_MOUSEMOTION:
            handle_mouse_motion(event);
            break;

        case SDL_MOUSEWHEEL:
            handle_mouse_wheel(event);
            break;

        case SDL_KEYDOWN: {
            SDL_Keymod mod = SDL_GetModState();

            if ((mod & KMOD_GUI) && event->key.keysym.sym == SDLK_c) {
                handle_copy();
            }
            else if ((mod & KMOD_GUI) && event->key.keysym.sym == SDLK_v) {
                handle_paste();
            }
            else if ((mod & KMOD_GUI) && event->key.keysym.sym == SDLK_g) {
                handle_g_pressed();
            }
            else if ((mod & KMOD_GUI) && event->key.keysym.sym == SDLK_s) {
                handle_s_pressed();
            }
            else if ((mod & KMOD_GUI) && event->key.keysym.sym == SDLK_l) {
                handle_l_pressed();
            }
            else if ((mod & KMOD_GUI) && event->key.keysym.sym == SDLK_r) {
                handle_r_pressed();
            }
            else if (event->key.keysym.sym == SDLK_BACKSPACE) {
                handle_backspace();
            }
            else if (event->key.keysym.sym == SDLK_ESCAPE) {
                void *nothing = NULL;
                handle_escape(nothing);
            }
            else if (event->key.keysym.sym == SDLK_RETURN || event->key.keysym.sym == SDLK_KP_ENTER) {
                void *nothing = NULL;
                handle_enter(nothing);
            }
            break;
        }

        case SDL_KEYUP:
            break;

        case SDL_QUIT:
            sim_state->is_running = 0;
            break;
    }
}

void handle_mouse_wheel(SDL_Event *event) {
    float old_zoom = sim_state->camera_zoom;
    sim_state->mouse_wheel = event->wheel.y;

    if (event->wheel.y > 0) {
        sim_state->camera_zoom *= 1.1f;
    }
    else if (event->wheel.y < 0) {
        sim_state->camera_zoom /= 1.1f;
    }

    // Clamp zoom
    if (sim_state->camera_zoom < 0.1f) sim_state->camera_zoom = 0.1f;
    if (sim_state->camera_zoom > 10.0f) sim_state->camera_zoom = 10.0f;

    // zoom to center
    float world_center_x = (WINDOW_WIDTH  / 2.0f) / old_zoom + sim_state->camera_x;
    float world_center_y = (WINDOW_HEIGHT / 2.0f) / old_zoom + sim_state->camera_y;

    sim_state->camera_x = world_center_x - (WINDOW_WIDTH  / 2.0f) / sim_state->camera_zoom;
    sim_state->camera_y = world_center_y - (WINDOW_HEIGHT / 2.0f) / sim_state->camera_zoom;
}

void handle_mouse_button_down(SDL_Event *event) {
    sim_state->mouse_x = event->button.x;
    sim_state->mouse_y = event->button.y;

    switch (event->button.button) {
        case SDL_BUTTON_LEFT:
            sim_state->left_mouse_down = 1;
            process_left_click();
            break;

        case SDL_BUTTON_RIGHT:
            sim_state->right_mouse_down = 1;
            process_right_click();
            break;

        case SDL_BUTTON_MIDDLE:
            sim_state->drag_offset_x = event->button.x;
            sim_state->drag_offset_y = event->button.y;
            sim_state->middle_mouse_down = 1;
            break;
    }
}


void handle_mouse_button_up(SDL_Event *event) {
    sim_state->mouse_x = event->button.x;
    sim_state->mouse_y = event->button.y;
    sim_state->drag_offset_x = 0;
    sim_state->drag_offset_y = 0;
    sim_state->is_node_dragging = 0;
    sim_state->is_camera_dragging = 0;

    switch (event->button.button) {
        case SDL_BUTTON_LEFT:
            sim_state->left_mouse_down = 0;
            sim_state->last_dragged_node = sim_state->dragged_node;
            sim_state->dragged_node = NULL;
            process_left_mouse_up();
            break;

        case SDL_BUTTON_RIGHT:
            sim_state->right_mouse_down = 0;
            process_right_mouse_up();
            break;

        case SDL_BUTTON_MIDDLE:
            sim_state->middle_mouse_down = 0;
            break;
    }
}


void handle_mouse_motion(SDL_Event *event) {
    sim_state->mouse_x = event->motion.x;
    sim_state->mouse_y = event->motion.y;

    if (sim_state->left_mouse_down) sim_state->is_node_dragging = 1;
    if (sim_state->middle_mouse_down) sim_state->is_camera_dragging = 1;

    if (sim_state->is_camera_dragging) {
        sim_state->camera_x -= (event->motion.x - sim_state->drag_offset_x) / sim_state->camera_zoom;
        sim_state->camera_y -= (event->motion.y - sim_state->drag_offset_y) / sim_state->camera_zoom;
        sim_state->drag_offset_x = event->motion.x;
        sim_state->drag_offset_y = event->motion.y;
    }

    process_mouse_motion();
}
