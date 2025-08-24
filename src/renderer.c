#include "renderer.h"

RenderContext *init_renderer()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return NULL;
    }

    if (TTF_Init() < 0)
    {
        printf("SDL_ttf could not initialize! TTF_Error: %s\n", TTF_GetError());
        SDL_Quit();
        return NULL;
    }

    RenderContext *context = malloc(sizeof(RenderContext));
    if (!context)
    {
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }

    context->window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    if (!context->window)
    {
        free(context);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    context->renderer = SDL_CreateRenderer(context->window, -1, SDL_RENDERER_ACCELERATED);

    if (!context->renderer)
    {
        SDL_DestroyWindow(context->window);
        free(context);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }

    context->font = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 16);
    if (!context->font)
    {
        printf("Failed to load font! TTF_Error: %s\n", TTF_GetError());
        SDL_DestroyRenderer(context->renderer);
        SDL_DestroyWindow(context->window);
        free(context);
        TTF_Quit();
        SDL_Quit();
        return NULL;
    }

    TTF_SetFontHinting(context->font, TTF_HINTING_LIGHT);


    context->image_cache.play_texture = IMG_LoadTexture(context->renderer, IMG_PATH_PLAY);
    context->image_cache.pause_texture = IMG_LoadTexture(context->renderer, IMG_PATH_PAUSE);
    context->image_cache.step_back_texture = IMG_LoadTexture(context->renderer, IMG_PATH_STEP_BACK);
    context->image_cache.step_forth_texture = IMG_LoadTexture(context->renderer, IMG_PATH_STEP_FORTH);
    context->image_cache.plus_texture = IMG_LoadTexture(context->renderer, IMG_PATH_PLUS);
    context->image_cache.minus_texture = IMG_LoadTexture(context->renderer, IMG_PATH_MINUS);
    context->image_cache.reload_texture = IMG_LoadTexture(context->renderer, IMG_PATH_RELOAD);
    context->image_cache.download_texture = IMG_LoadTexture(context->renderer, IMG_PATH_DOWNLOAD);
    context->image_cache.trash_texture = IMG_LoadTexture(context->renderer, IMG_PATH_TRASH);
    context->image_cache.switch_on_texture = IMG_LoadTexture(context->renderer, IMG_PATH_SWITCH_ON);
    context->image_cache.switch_off_texture = IMG_LoadTexture(context->renderer, IMG_PATH_SWITCH_OFF);
    context->image_cache.light_on_texture = IMG_LoadTexture(context->renderer, IMG_PATH_LIGHT_ON);
    context->image_cache.light_off_texture = IMG_LoadTexture(context->renderer, IMG_PATH_LIGHT_OFF);
    context->image_cache.arrows_texture = IMG_LoadTexture(context->renderer, IMG_PATH_ARROWS);
    context->image_cache.circle_texture = IMG_LoadTexture(context->renderer, IMG_PATH_CIRCLE);

    context->text_cache.not_texture = text_to_texture(context, "NOT", NULL);
    context->text_cache.and_texture = text_to_texture(context, "AND", NULL);
    context->text_cache.or_texture = text_to_texture(context, "OR", NULL);
    context->text_cache.nor_texture = text_to_texture(context, "NOR", NULL);
    context->text_cache.nand_texture = text_to_texture(context, "NAND", NULL);
    context->text_cache.xor_texture = text_to_texture(context, "XOR", NULL);
    context->text_cache.xnor_texture = text_to_texture(context, "XNOR", NULL);
    context->text_cache.switch_texture = text_to_texture(context, "SWITCH", NULL);
    context->text_cache.light_texture = text_to_texture(context, "LIGHT", NULL);
    context->text_cache.note_texture = text_to_texture(context, "NOTE", NULL);

    if (!context->image_cache.play_texture || !context->image_cache.pause_texture ||
        !context->image_cache.step_back_texture || !context->image_cache.step_forth_texture ||
        !context->image_cache.plus_texture || !context->image_cache.minus_texture ||
        !context->image_cache.reload_texture || !context->image_cache.download_texture ||
        !context->image_cache.trash_texture || !context->image_cache.switch_on_texture ||
        !context->image_cache.switch_off_texture || !context->image_cache.light_on_texture ||
        !context->image_cache.light_off_texture || !context->image_cache.arrows_texture ||
        !context->image_cache.circle_texture || !context->text_cache.not_texture ||
        !context->text_cache.and_texture || !context->text_cache.or_texture ||
        !context->text_cache.nor_texture || !context->text_cache.nand_texture ||
        !context->text_cache.xor_texture || !context->text_cache.xnor_texture ||
        !context->text_cache.switch_texture || !context->text_cache.light_texture ||
        !context->text_cache.note_texture )
    {
        printf("Failed to load some textures! IMG_Error: %s\n", IMG_GetError());
        cleanup_renderer(context);
        return NULL;
    }

    assert(context->window != NULL);
    assert(context->renderer != NULL);
    assert(context->font != NULL);

    return context;
}

void clear_screen(RenderContext *context)
{
    assert(context != NULL);
    SDL_SetRenderDrawColor(context->renderer, 100, 100, 100, 255);
    SDL_RenderClear(context->renderer);
}

void present_screen(RenderContext *context)
{
    assert(context != NULL);
    SDL_RenderPresent(context->renderer);
}

void cleanup_renderer(RenderContext *context)
{
    assert(context != NULL);
    TTF_CloseFont(context->font);
    SDL_DestroyRenderer(context->renderer);
    SDL_DestroyWindow(context->window);

    SDL_DestroyTexture(context->image_cache.play_texture);
    SDL_DestroyTexture(context->image_cache.pause_texture);
    SDL_DestroyTexture(context->image_cache.step_back_texture);
    SDL_DestroyTexture(context->image_cache.step_forth_texture);
    SDL_DestroyTexture(context->image_cache.plus_texture);
    SDL_DestroyTexture(context->image_cache.minus_texture);
    SDL_DestroyTexture(context->image_cache.reload_texture);
    SDL_DestroyTexture(context->image_cache.download_texture);
    SDL_DestroyTexture(context->image_cache.trash_texture);
    SDL_DestroyTexture(context->image_cache.switch_on_texture);
    SDL_DestroyTexture(context->image_cache.switch_off_texture);
    SDL_DestroyTexture(context->image_cache.light_on_texture);
    SDL_DestroyTexture(context->image_cache.light_off_texture);
    SDL_DestroyTexture(context->image_cache.arrows_texture);
    SDL_DestroyTexture(context->image_cache.circle_texture);

    free(context);
    TTF_Quit();
    SDL_Quit();
}

TextChacheElement *text_to_texture(RenderContext *context, char *text, SDL_Color *color) {
    TextChacheElement *tce= malloc(sizeof(TextChacheElement));

    SDL_Surface *surface = TTF_RenderText_Blended(
        context->font,
        text,
        color ? *color : (SDL_Color){255, 255, 255, 255}
    );

    tce->texture  = SDL_CreateTextureFromSurface(context->renderer, surface);
    if (!tce->texture) {
        printf("Failed to create texture! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }

    tce->rect = malloc(sizeof(Float_Rect));
    tce->rect->x = 0;
    tce->rect->y = 0;
    tce->rect->w = (int)(surface->w);
    tce->rect->h = (int)(surface->h);

    return tce;
}

void render_text(RenderContext *context, char *text, int x, int y, SDL_Color *color, float zoom) {
    assert(context != NULL);
    assert(text != NULL);
    assert(context->font != NULL);

    if (color == NULL) {
        SDL_Color default_color = {255, 255, 255, 255};
        color = &default_color;
    }

    SDL_Surface *surface = TTF_RenderText_Blended(context->font, text, *color);
    if (!surface) {
        printf("Failed to render text! TTF_Error: %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(context->renderer, surface);
    if (!texture) {
        printf("Failed to create texture! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect = {
        x,
        y,
        (int)(surface->w * zoom),
        (int)(surface->h * zoom)
    };

    SDL_RenderCopy(context->renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void render_img(RenderContext *context, SDL_Texture *texture, SDL_Rect *rect) {
    assert(context != NULL);
    assert(texture != NULL);
    assert(rect != NULL);
    assert(rect->w > 0 && rect->h > 0 && "Image dimensions must be positive");

    SDL_RenderCopy(context->renderer, texture, NULL, rect);
}

void render_breadcrumb(RenderContext *context) {
    assert(context != NULL);

    if (sim_state->subnode_window_parent == NULL) return;

    char breadcrumb[1024] = "Layer0";

    int depth = 0;
    Node *current = sim_state->subnode_window_parent;
    while (current != NULL) {
        depth++;
        current = current->parent;
    }

    Node **path = malloc(depth * sizeof(Node*));
    if (!path) return;

    current = sim_state->subnode_window_parent;
    for (int i = depth - 1; i >= 0; i--) {
        path[i] = current;
        current = current->parent;
    }

    strcpy(breadcrumb, "Layer0");
    for (int i = 0; i < depth; i++) {
        strcat(breadcrumb, " < ");
        strcat(breadcrumb, path[i]->name);
    }

    free(path);

    SDL_Color breadcrumb_color = {200, 200, 200, 255};
    int text_width, text_height;

    if (TTF_SizeText(context->font, breadcrumb, &text_width, &text_height) == 0) {
        render_text(context, breadcrumb, 120, TOP_BAR_HEIGHT + 15, &breadcrumb_color, 1.0f);
    }
}

void render_top_bar(RenderContext *context) {
    assert(context != NULL);
    SDL_SetRenderDrawColor(context->renderer, 40, 40, 40, 255);
    SDL_Rect top_bar = {0, 0, WINDOW_WIDTH, TOP_BAR_HEIGHT};
    SDL_RenderFillRect(context->renderer, &top_bar);

    for (int i = 0; i < sim_state->buttons->size; i++) {
        Button *button = array_get(sim_state->buttons, i);

        if (strncmp(button->name, IMG_PATH_PLAY, 23) == 0 && !sim_state->is_paused) { button->name = IMG_PATH_PAUSE; }
        if (strncmp(button->name, IMG_PATH_PAUSE, 24) == 0 && sim_state->is_paused) { button->name = IMG_PATH_PLAY; }

        assert(button != NULL);
        render_button(context, button);
    }

    render_breadcrumb(context);
}

void render_button(RenderContext *context, Button *button) {
    assert(context != NULL);
    assert(button != NULL);
    assert(button->name != NULL);

    int text_width, text_height;
    SDL_Rect button_rect = float_rect_to_sdl_rect(&button->rect);
    if (strcmp(button->name, IMG_PATH_PLAY) == 0) {
        render_img(context, context->image_cache.play_texture, &button_rect);
    } else if (strcmp(button->name, IMG_PATH_PAUSE) == 0) {
        render_img(context, context->image_cache.pause_texture, &button_rect);
    } else if (strcmp(button->name, IMG_PATH_STEP_BACK) == 0) {
        render_img(context, context->image_cache.step_back_texture, &button_rect);
    } else if (strcmp(button->name, IMG_PATH_STEP_FORTH) == 0) {
        render_img(context, context->image_cache.step_forth_texture, &button_rect);
    } else if (strcmp(button->name, IMG_PATH_RELOAD) == 0) {
        render_img(context, context->image_cache.reload_texture, &button_rect);
    }
    else {
        SDL_SetRenderDrawColor(context->renderer, 20, 20, 20, 255);
        SDL_RenderFillRect(context->renderer, &button_rect);

        if (TTF_SizeText(context->font, button->name, &text_width, &text_height) == 0) {
            int text_x = button_rect.x + (button_rect.w - text_width) / 2;
            int text_y = button_rect.y + (button_rect.h - text_height) / 2;

            render_text(context, button->name, text_x, text_y, NULL, 1);
        }
        else {
            render_text(context, button->name, button_rect.x, button_rect.y, NULL, 1);
        }
    }
}

void render_knife_stroke(RenderContext *context) {
    assert(context != NULL);
    assert(sim_state != NULL);
    SDL_SetRenderDrawColor(context->renderer, 255, 255, 255, 255);

    for (int i = 0; i < sim_state->knife_stroke->size - 1; i++) {
        SDL_Point *p1 = array_get(sim_state->knife_stroke, i);
        SDL_Point *p2 = array_get(sim_state->knife_stroke, i + 1);

        float x1, y1, x2, y2;
        world_point_to_screen(p1->x, p1->y, &x1, &y1);
        world_point_to_screen(p2->x, p2->y, &x2, &y2);

        SDL_RenderDrawLine(context->renderer, x1, y1, x2, y2);
    }
}

void render_selected_node_outline(RenderContext *context, Node *node) {
    assert(context != NULL);
    assert(node != NULL);
    assert(sim_state != NULL);

    SDL_Rect selection_rect = float_rect_to_sdl_rect(&node->rect);

    selection_rect.x -= 2;
    selection_rect.y -= 2;
    selection_rect.w += 4;
    selection_rect.h += 4;

    float new_x, new_y;
    world_point_to_screen(selection_rect.x, selection_rect.y, &new_x, &new_y);
    int new_w = (int)(selection_rect.w * sim_state->camera_zoom);
    int new_h = (int)(selection_rect.h * sim_state->camera_zoom);
    SDL_Rect dest = {new_x, new_y, new_w, new_h};

    SDL_SetRenderDrawColor(context->renderer, 225, 225, 225, 255);
    SDL_RenderDrawRect(context->renderer, &dest);
}

void render_pins(RenderContext *context, Node *node) {
    SDL_Color hover_color = {0, 180, 255, 255};

    int num_inputs = node->inputs->size;
    for (int i = 0; i < num_inputs; i++) {
        Pin *pin = array_get(node->inputs, i);

        Float_Rect pin_rect;
        world_point_to_screen(node->rect.x + pin->x, node->rect.y + pin->y, &pin_rect.x, &pin_rect.y);
        pin_rect.w = (PIN_SIZE * sim_state->camera_zoom);
        pin_rect.h = (PIN_SIZE * sim_state->camera_zoom);
        SDL_Rect pin_screen_rect = float_rect_to_sdl_rect(&pin_rect);

        if (sim_state->hovered_pin == pin) {
            SDL_SetTextureColorMod(context->image_cache.circle_texture, hover_color.r, hover_color.g, hover_color.b);
            char* connected_light_name = find_connected_light_switch_name(pin);
            if (connected_light_name != NULL) {
                SDL_Surface *surface = TTF_RenderText_Blended(context->font, connected_light_name, hover_color);

                int text_x = pin_screen_rect.x - (surface->w*sim_state->camera_zoom) - 5;
                int text_y = pin_screen_rect.y - (PIN_SIZE * sim_state->camera_zoom) / 2;
                render_text(context, connected_light_name, text_x, text_y, &hover_color, 1);
            }
        }
        else if (pin->state) SDL_SetTextureColorMod(context->image_cache.circle_texture, 0, 200, 103);
        else SDL_SetTextureColorMod(context->image_cache.circle_texture, 0, 0, 0);
        SDL_RenderCopy(context->renderer, context->image_cache.circle_texture, NULL, &pin_screen_rect);
    }

    int num_outputs = node->outputs->size;
    for (int i = 0; i < num_outputs; i++) {
        Pin *pin = array_get(node->outputs, i);

        Float_Rect pin_rect;
        world_point_to_screen(node->rect.x + pin->x, node->rect.y + pin->y, &pin_rect.x, &pin_rect.y);
        pin_rect.w = (PIN_SIZE * sim_state->camera_zoom);
        pin_rect.h = (PIN_SIZE * sim_state->camera_zoom);
        SDL_Rect pin_screen_rect = float_rect_to_sdl_rect(&pin_rect);

        if (sim_state->hovered_pin == pin) {
            SDL_SetTextureColorMod(context->image_cache.circle_texture, hover_color.r, hover_color.g, hover_color.b);
            char* connected_light_name = find_connected_light_switch_name(pin);
            if (connected_light_name != NULL) {
                int text_x = pin_screen_rect.x + pin_screen_rect.w + 5;
                int text_y = pin_screen_rect.y - (PIN_SIZE * sim_state->camera_zoom) / 2;
                render_text(context, connected_light_name, text_x, text_y, &hover_color, 1);
            }
        }
        else if (pin->state) SDL_SetTextureColorMod(context->image_cache.circle_texture, 0, 200, 103);
        else SDL_SetTextureColorMod(context->image_cache.circle_texture, 0, 0, 0);
        SDL_RenderCopy(context->renderer, context->image_cache.circle_texture, NULL, &pin_screen_rect);
    }
}

void render_node(RenderContext *context, Node *node) {
    assert(context != NULL);
    assert(node != NULL);
    assert(sim_state != NULL);

    Float_Rect node_rect = node->rect;
    if (sim_state->dragged_node != NULL && sim_state->dragged_node == node) {
        render_selected_node_outline(context, node);
    }

    float new_x, new_y;
    world_point_to_screen(node_rect.x, node_rect.y, &new_x, &new_y);
    int new_w = (int)(node_rect.w * sim_state->camera_zoom);
    int new_h = (int)(node_rect.h * sim_state->camera_zoom);
    SDL_Rect dest = {new_x, new_y, new_w, new_h};

    if (node->operation == switchNode) {
        Pin *p = array_get(node->outputs, 0);
        if (p->state) render_img(context, context->image_cache.switch_on_texture, &dest);
        else render_img(context, context->image_cache.switch_off_texture, &dest);
    } else if (node->operation == lightNode) {
        Pin *p = array_get(node->inputs, 0);
        if (p->state) render_img(context, context->image_cache.light_on_texture, &dest);
        else render_img(context, context->image_cache.light_off_texture, &dest);
    } else {
        if(node->sub_nodes != NULL) SDL_SetRenderDrawColor(context->renderer, 255, 165, 0, 255);
        else if (node->operation == andGate) SDL_SetRenderDrawColor(context->renderer, 0, 128, 0, 255);
        else if (node->operation == orGate) SDL_SetRenderDrawColor(context->renderer, 165, 165, 0, 255);
        else if (node->operation == notGate) SDL_SetRenderDrawColor(context->renderer, 128, 0, 0, 255);
        else if (node->operation == norGate) SDL_SetRenderDrawColor(context->renderer, 128, 0, 128, 255);
        else if (node->operation == nandGate) SDL_SetRenderDrawColor(context->renderer, 0, 0, 128, 255);
        else if (node->operation == xorGate) SDL_SetRenderDrawColor(context->renderer, 255, 20, 147, 255);
        else if (node->operation == xnorGate) SDL_SetRenderDrawColor(context->renderer, 0, 139, 139, 255);
        else  SDL_SetRenderDrawColor(context->renderer, 70, 70, 70, 255);

        SDL_RenderFillRect(context->renderer, &dest);
    }

    SDL_SetTextureAlphaMod(context->image_cache.circle_texture, 255);
    render_pins(context, node);

    TextChacheElement *text_texture = NULL;

    if (strcmp(node->name, "NOT") == 0) text_texture = context->text_cache.not_texture;
    else if (strcmp(node->name, "AND") == 0) text_texture = context->text_cache.and_texture;
    else if (strcmp(node->name, "OR") == 0) text_texture = context->text_cache.or_texture;
    else if (strcmp(node->name, "XOR") == 0) text_texture = context->text_cache.xor_texture;
    else if (strcmp(node->name, "XNOR") == 0) text_texture = context->text_cache.xnor_texture;
    else if (strcmp(node->name, "NOR") == 0) text_texture = context->text_cache.nor_texture;
    else if (strcmp(node->name, "NAND") == 0) text_texture = context->text_cache.nand_texture;
    else if (strcmp(node->name, "SWITCH") == 0) text_texture = context->text_cache.switch_texture;
    else if (strcmp(node->name, "LIGHT") == 0) text_texture = context->text_cache.light_texture;
    else if (strcmp(node->name, "NOTE") == 0) text_texture = context->text_cache.note_texture;
    else text_texture = text_to_texture(context, node->name, NULL);

    if (text_texture != NULL) {

        Float_Rect screen_node_rect;
        world_rect_to_screen(&node->rect, &screen_node_rect);

        int text_width = (int)(text_texture->rect->w * sim_state->camera_zoom);
        int text_height = (int)(text_texture->rect->h * sim_state->camera_zoom);

        int text_x = screen_node_rect.x + (screen_node_rect.w - text_width) / 2;
        int text_y = screen_node_rect.y + (screen_node_rect.h - text_height) / 2;
        if (node->operation == switchNode) text_x = screen_node_rect.x - text_width - 5;
        if (node->operation == lightNode) text_x = screen_node_rect.x + screen_node_rect.w + 5;

        SDL_Rect render_rect = {
            text_x,
            text_y,
            text_width,
            text_height};

        SDL_RenderCopy(context->renderer, text_texture->texture, NULL, &render_rect);
    }
    else {
        printf("Error rendering text texture\n");
    }
}

void render_connection_dragging(RenderContext *context) {
    float world_x, world_y;
    world_point_to_screen(sim_state->last_connection_point->pos.x, sim_state->last_connection_point->pos.y, &world_x, &world_y);

    SDL_SetRenderDrawColor(context->renderer, 0, 0, 30, 255);

    render_connection_branch(context, sim_state->new_connection);
    render_connection_points(context, sim_state->new_connection);

    int thickness = (int)(4 * sim_state->camera_zoom);
    for (int t = -thickness / 2; t <= thickness / 2; t++) {
        SDL_RenderDrawLine(context->renderer, (int)world_x, (int)(world_y + t), sim_state->mouse_x, sim_state->mouse_y + t);
        SDL_RenderDrawLine(context->renderer, (int)(world_x + t), (int)world_y, sim_state->mouse_x + t, sim_state->mouse_y);
    }
}

void render_connection(RenderContext *context, Connection *con) {
    assert(context != NULL);
    assert(con != NULL);

    if (con->state) {
        SDL_SetRenderDrawColor(context->renderer, 0, 200, 103, 255);
    } else {
        SDL_SetRenderDrawColor(context->renderer, 0, 0, 30, 255);
    }

    render_connection_branch(context, con);
    render_connection_points(context, con);
}

void render_connection_branch(RenderContext *context, Connection *con) {
    int thickness = (int)(4 * sim_state->camera_zoom);
    for (int i = 0; i < con->points->size; i++) {
        Connection_point *p1 = array_get(con->points, i);
        float sx1, sy1;
        world_point_to_screen(p1->pos.x, p1->pos.y, &sx1, &sy1);

        for (int j = 0; j < p1->neighbors->size; j++) {
            Connection_point *p2 = array_get(p1->neighbors, j);

            if (p1 < p2) {
                float sx2, sy2;
                world_point_to_screen(p2->pos.x, p2->pos.y, &sx2, &sy2);

                for (int t = -thickness / 2; t <= thickness / 2; t++) {
                    SDL_RenderDrawLine(context->renderer, (int)sx1, (int)(sy1 + t), (int)sx2, (int)(sy2 + t));
                    SDL_RenderDrawLine(context->renderer, (int)(sx1 + t), (int)sy1, (int)(sx2 + t), (int)sy2);
                }
            }
        }
    }
}

void render_connection_points(RenderContext *context, Connection *con) {
    for (int i = 0; i < con->points->size; i++) {
        Connection_point *p = array_get(con->points, i);

        float sx, sy;
        world_point_to_screen(p->pos.x, p->pos.y, &sx, &sy);

        SDL_Rect pin_rect;
        int pin_size = (int)(CONNECTION_POINT_SIZE * sim_state->camera_zoom);
        pin_rect.w = pin_size;
        pin_rect.h = pin_size;
        pin_rect.x = sx - pin_size / 2;
        pin_rect.y = sy - pin_size / 2;

        if (sim_state->hovered_connection_point == p) SDL_SetTextureColorMod(context->image_cache.circle_texture, 255, 255, 60);
        else SDL_SetTextureColorMod(context->image_cache.circle_texture, 0, 0, 30);

        for (int j = 0; j < sim_state->selected_connection_points->size; j++) {
            Connection_point *current = array_get(sim_state->selected_connection_points, j);
            if (current == p) {
                SDL_SetTextureColorMod(context->image_cache.circle_texture, 225, 225, 225);
                break;
            }
        }

        SDL_RenderCopy(context->renderer, context->image_cache.circle_texture, NULL, &pin_rect);
    }
}

void render_origin_marker(RenderContext *context) {
    assert(context != NULL);
    assert(sim_state != NULL);

    float screen_x, screen_y;
    world_point_to_screen(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, &screen_x, &screen_y);

    SDL_Renderer *renderer = context->renderer;

    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);

    int crosshair_length = 5;
    int crosshair_thickness = 1;

    for (int t = -crosshair_thickness/2; t <= crosshair_thickness/2; t++) {
        SDL_RenderDrawLine(
            renderer,
            (int)(screen_x - crosshair_length), (int)(screen_y + t),
            (int)(screen_x + crosshair_length), (int)(screen_y + t)
        );
    }

    for (int t = -crosshair_thickness/2; t <= crosshair_thickness/2; t++) {
        SDL_RenderDrawLine(
            renderer,
            (int)(screen_x + t), (int)(screen_y - crosshair_length),
            (int)(screen_x + t), (int)(screen_y + crosshair_length)
        );
    }
}

void render_selection_box(RenderContext *context) {
    assert(context != NULL);
    assert(sim_state != NULL);

    SDL_Renderer *renderer = context->renderer;
    assert(renderer != NULL);

    SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
    SDL_Rect draw_rect_1 = float_rect_to_sdl_rect(&sim_state->selection_box);
    SDL_RenderFillRect(renderer, &draw_rect_1);

    SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
    SDL_Rect draw_rect_2 = float_rect_to_sdl_rect(&sim_state->selection_box);
    SDL_RenderDrawRect(renderer, &draw_rect_2);
}

void render_text_input(RenderContext *context, TextInput *input) {
    SDL_SetRenderDrawColor(context->renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(context->renderer, &input->rect);

    if (strlen(input->text) > 0) {
        SDL_Color textColor = {0, 0, 0, 255};
        render_text(context, input->text, input->rect.x + 5, input->rect.y + 5, &textColor, 1.0f);
    }
}

void render_popup(RenderContext *context) {
    assert(context != NULL);
    assert(sim_state != NULL);
    assert(sim_state->popup_state != NULL);

    SDL_SetRenderDrawBlendMode(context->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(context->renderer, 0, 0, 0, 80);
    SDL_Rect fullscreen_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(context->renderer, &fullscreen_rect);

    PopupState *pop_state = sim_state->popup_state;

    SDL_SetRenderDrawColor(context->renderer, 45, 45, 45, 255);
    SDL_RenderFillRect(context->renderer, &pop_state->rect);

    SDL_SetRenderDrawColor(context->renderer, 80, 80, 80, 255);
    SDL_RenderDrawRect(context->renderer, &pop_state->rect);

    SDL_Color title_color = {255, 255, 255, 255};
    render_text(context, pop_state->title, pop_state->rect.x + PADDING, pop_state->rect.y + PADDING, &title_color, 1.0f);

    for (int i = 0; i < pop_state->buttons->size; i++) {
        Button * btn = array_get(pop_state->buttons, i);
        render_button(context, btn);
    }
    render_text_input(context, &pop_state->name_input);

    render_button(context, pop_state->enter_button);
    render_button(context, pop_state->esc_button);
}

void render(RenderContext *context)
{
    assert(context != NULL);
    assert(sim_state != NULL);
    assert(sim_state->buttons != NULL);
    assert(sim_state->nodes != NULL);

    clear_screen(context);

    render_origin_marker(context);
    render_knife_stroke(context);
    render_selection_box(context);

    if (sim_state->subnode_window_parent != NULL) {
        // Render subnode view
        Node *parent_node = sim_state->subnode_window_parent;

        if (parent_node->sub_connections != NULL) {
            for (int i = 0; i < parent_node->sub_connections->size; i++) {
                Connection *connection = array_get(parent_node->sub_connections, i);
                assert(connection != NULL);
                render_connection(context, connection);
            }
        }

        if (parent_node->sub_nodes != NULL) {
            Node *last_node = NULL;

            for (int i = 0; i < parent_node->sub_nodes->size; i++) {
                Node *node = array_get(parent_node->sub_nodes, i);
                assert(node != NULL);

                if (node == sim_state->dragged_node) {
                    last_node = node;
                    continue;
                }
                render_node(context, node);
            }
            if (last_node != NULL) render_node(context, last_node);
        }

        if (sim_state->subnode_window_button != NULL) {
            render_button(context, sim_state->subnode_window_button);
        }
    }
    else {
        // Normal view
        for (int i = 0; i < sim_state->connections->size; i++) {
            Connection *connection = array_get(sim_state->connections, i);
            assert(connection != NULL);

            render_connection(context, connection);
        }

        Node *last_node = NULL;

        for (int i = 0; i < sim_state->nodes->size; i++) {
            Node *node = array_get(sim_state->nodes, i);
            assert(node != NULL);

            if (node == sim_state->dragged_node) {
                last_node = node;
                continue;
            }
            render_node(context, node);
        }
        if (last_node != NULL) render_node(context, last_node);

    }

    for (int i = 0; i < sim_state->selected_nodes->size; i++) {
        Node *node = array_get(sim_state->selected_nodes, i);
        assert(node != NULL);

        render_selected_node_outline(context, node);
    }

    if (sim_state->is_cable_dragging) render_connection_dragging(context);

    render_top_bar(context);

    if (sim_state->popup_state != NULL) render_popup(context);

    present_screen(context);
}
