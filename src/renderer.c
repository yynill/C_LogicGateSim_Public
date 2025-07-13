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

    context->circle_texture = IMG_LoadTexture(context->renderer, "assets/images/circle.png");
    if (!context->circle_texture)
    {
        printf("Failed to load circle texture! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(context->renderer);
        SDL_DestroyWindow(context->window);
        free(context);
        TTF_Quit();
        SDL_Quit();
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
    SDL_DestroyTexture(context->circle_texture);
    free(context);
    TTF_Quit();
    SDL_Quit();
}

void screen_to_world(int screen_x, int screen_y, float *world_x, float *world_y) {
    *world_x = (screen_x / sim_state->camera_zoom) + sim_state->camera_x;
    *world_y = (screen_y / sim_state->camera_zoom) + sim_state->camera_y;
}

void world_to_screen(float world_x, float world_y, int *screen_x, int *screen_y) {
    *screen_x = (int)((world_x - sim_state->camera_x) * sim_state->camera_zoom);
    *screen_y = (int)((world_y - sim_state->camera_y) * sim_state->camera_zoom);
}

void screen_rect_to_world(const SDL_Rect *screen, SDL_Rect *out_world) {
    assert(screen != NULL);
    assert(out_world != NULL);

    float world_x1, world_y1;
    float world_x2, world_y2;

    // top left corner
    screen_to_world(screen->x, screen->y, &world_x1, &world_y1);

    // botom right cornder
    screen_to_world(screen->x + screen->w, screen->y + screen->h, &world_x2, &world_y2);

    out_world->x = (int)fminf(world_x1, world_x2);
    out_world->y = (int)fminf(world_y1, world_y2);
    out_world->w = (int)fabsf(world_x2 - world_x1);
    out_world->h = (int)fabsf(world_y2 - world_y1);
}

void world_rect_to_screen(const SDL_Rect *world, SDL_Rect *out_screen) {
    assert(world != NULL);
    assert(out_screen != NULL);

    int screen_x1, screen_y1;
    int screen_x2, screen_y2;

    world_to_screen(world->x, world->y, &screen_x1, &screen_y1);
    world_to_screen(world->x + world->w, world->y + world->h, &screen_x2, &screen_y2);

    out_screen->x = fmin(screen_x1, screen_x2);
    out_screen->y = fmin(screen_y1, screen_y2);
    out_screen->w = abs(screen_x2 - screen_x1);
    out_screen->h = abs(screen_y2 - screen_y1);
}


void render_text(RenderContext *context, const char *text, int x, int y, SDL_Color *color, float zoom) {
    assert(context != NULL);
    assert(text != NULL);
    assert(context->font != NULL);

    if (zoom < 0.8f) return;
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

void render_img(RenderContext *context, const char *path, SDL_Rect *rect) {
    assert(context != NULL);
    assert(path != NULL);
    assert(rect != NULL);
    assert(rect->w > 0 && rect->h > 0 && "Image dimensions must be positive");

    SDL_Surface *surface = IMG_Load(path);
    if (!surface)
    {
        printf("Failed to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
        return;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(context->renderer, surface);
    if (!texture)
    {
        printf("Failed to create texture! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_RenderCopy(context->renderer, texture, NULL, rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void render_top_bar(RenderContext *context) {
    assert(context != NULL);
    SDL_SetRenderDrawColor(context->renderer, 40, 40, 40, 255);
    SDL_Rect top_bar = {0, 0, WINDOW_WIDTH, TOP_BAR_HEIGHT};
    SDL_RenderFillRect(context->renderer, &top_bar);
}

void render_button(RenderContext *context, Button *button) {
    assert(context != NULL);
    assert(button != NULL);
    assert(button->name != NULL);
    assert(button->rect.w > 0 && button->rect.h > 0);

    int text_width, text_height;
    if (strncmp(button->name, "/", 1) == 0) {
        render_img(context, button->name, &button->rect);
    }
    else {
        SDL_SetRenderDrawColor(context->renderer, 20, 20, 20, 255);
        SDL_RenderFillRect(context->renderer, &button->rect);

        if (TTF_SizeText(context->font, button->name, &text_width, &text_height) == 0) {
            int text_x = button->rect.x + (button->rect.w - text_width) / 2;
            int text_y = button->rect.y + (button->rect.h - text_height) / 2;

            render_text(context, button->name, text_x, text_y, NULL, 1);
        }
        else {
            render_text(context, button->name, button->rect.x, button->rect.y, NULL, 1);
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

        int x1, y1, x2, y2;
        world_to_screen(p1->x, p1->y, &x1, &y1);
        world_to_screen(p2->x, p2->y, &x2, &y2);

        SDL_RenderDrawLine(context->renderer, x1, y1, x2, y2);
    }
}

void render_selected_node_outline(RenderContext *context, Node *node) {
    assert(context != NULL);
    assert(node != NULL);
    assert(sim_state != NULL);

    SDL_Rect selection_rect = node->rect;

    selection_rect.x -= 2;
    selection_rect.y -= 2;
    selection_rect.w += 4;
    selection_rect.h += 4;

    int new_x, new_y;
    world_to_screen(selection_rect.x, selection_rect.y, &new_x, &new_y);
    int new_w = (int)(selection_rect.w * sim_state->camera_zoom);
    int new_h = (int)(selection_rect.h * sim_state->camera_zoom);
    SDL_Rect dest = {new_x, new_y, new_w, new_h};

    SDL_SetRenderDrawColor(context->renderer, 225, 225, 225, 255);
    SDL_RenderDrawRect(context->renderer, &dest);
}

void render_single_node(RenderContext *context, Node *node) {
    SDL_Rect node_rect = node->rect;
    if (sim_state->dragged_node != NULL && sim_state->dragged_node == node) {
        render_selected_node_outline(context, node);
    }

    int new_x, new_y;
    world_to_screen(node_rect.x, node_rect.y, &new_x, &new_y);
    int new_w = (int)(node_rect.w * sim_state->camera_zoom);
    int new_h = (int)(node_rect.h * sim_state->camera_zoom);
    SDL_Rect dest = {new_x, new_y, new_w, new_h};

    if (node->operation == switchNode) {
        Pin *p = array_get(node->outputs, 0);
        if (p->state) render_img(context, "/assets/images/switch_on.png", &dest);
        else render_img(context, "/assets/images/switch_off.png", &dest);
    } else if (node->operation == lightNode) {
        Pin *p = array_get(node->inputs, 0);
        if (p->state) render_img(context, "/assets/images/light_on.png", &dest);
        else render_img(context, "/assets/images/light_off.png", &dest);
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

    SDL_SetTextureAlphaMod(context->circle_texture, 255);
    render_pins(context, node);

    if (node->operation != switchNode && node->operation != lightNode) {
        int text_width, text_height;
        if (TTF_SizeText(context->font, node->name, &text_width, &text_height) == 0) {
            int text_x, text_y;
            world_to_screen( node->rect.x, node->rect.y, &text_x, &text_y);
            text_x += (int)(((node->rect.w - text_width) * sim_state->camera_zoom) / 2);
            text_y += (int)(((node->rect.h - text_height) * sim_state->camera_zoom) / 2);
            render_text(context, node->name, text_x, text_y, NULL, sim_state->camera_zoom);
        } else {
            render_text(context, node->name, node->rect.x, node->rect.y, NULL, sim_state->camera_zoom);
        }
    }
}

void render_pins(RenderContext *context, Node *node) {
    int num_inputs = node->inputs->size;
    for (int i = 0; i < num_inputs; i++) {
        Pin *pin = array_get(node->inputs, i);

        SDL_Rect pin_rect;
        world_to_screen(node->rect.x + pin->x, node->rect.y + pin->y, &pin_rect.x, &pin_rect.y);
        pin_rect.w = (int)(PIN_SIZE * sim_state->camera_zoom);
        pin_rect.h = (int)(PIN_SIZE * sim_state->camera_zoom);

        if (sim_state->hovered_pin == pin) SDL_SetTextureColorMod(context->circle_texture, 255, 60, 60);
        else if (pin->state) SDL_SetTextureColorMod(context->circle_texture, 0, 200, 103);
        else SDL_SetTextureColorMod(context->circle_texture, 0, 0, 0);
        SDL_RenderCopy(context->renderer, context->circle_texture, NULL, &pin_rect);
    }

    int num_outputs = node->outputs->size;
    for (int i = 0; i < num_outputs; i++) {
        Pin *pin = array_get(node->outputs, i);

        SDL_Rect pin_rect;
        world_to_screen(node->rect.x + pin->x, node->rect.y + pin->y, &pin_rect.x, &pin_rect.y);
        pin_rect.w = (int)(PIN_SIZE * sim_state->camera_zoom);
        pin_rect.h = (int)(PIN_SIZE * sim_state->camera_zoom);

        if (sim_state->hovered_pin == pin) SDL_SetTextureColorMod(context->circle_texture, 255, 60, 60);
        else if (pin->state) SDL_SetTextureColorMod(context->circle_texture, 0, 200, 103);
        else SDL_SetTextureColorMod(context->circle_texture, 0, 0, 0);
        SDL_RenderCopy(context->renderer, context->circle_texture, NULL, &pin_rect);
    }
}

void render_node(RenderContext *context, Node *node) {
    assert(context != NULL);
    assert(node != NULL);
    assert(sim_state != NULL);

    if (node->sub_nodes == NULL) {
        render_single_node(context, node);
        return;
    }

    if (!node->is_expanded) {
        render_single_node(context, node);
        return;
    }

    // render group nodes
    SDL_Rect outline = node->outline_rect;

    int outline_screen_x, outline_screen_y;
    world_to_screen(outline.x, outline.y, &outline_screen_x, &outline_screen_y);
    int outline_screen_w = (int)(outline.w * sim_state->camera_zoom);
    int outline_screen_h = (int)(outline.h * sim_state->camera_zoom);
    SDL_Rect outline_screen_rect = {outline_screen_x, outline_screen_y, outline_screen_w, outline_screen_h};

    render_text(context, node->name, outline_screen_rect.x + 32 * sim_state->camera_zoom, outline_screen_rect.y - 32 * sim_state->camera_zoom, NULL, sim_state->camera_zoom * 1.3);

    SDL_Rect arrow_rect;
    world_rect_to_screen(&node->rect, &arrow_rect);

    render_img(context, "/assets/images/4_arrows.png", &arrow_rect);

    SDL_SetRenderDrawColor(context->renderer, 255, 165, 0, 128);
    SDL_SetRenderDrawBlendMode(context->renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(context->renderer, &outline_screen_rect);
    SDL_SetRenderDrawColor(context->renderer, 255, 165, 0, 255);
    SDL_RenderDrawRect(context->renderer, &outline_screen_rect);
    SDL_SetRenderDrawBlendMode(context->renderer, SDL_BLENDMODE_NONE);

    for (int i = 0 ; i < node->sub_connections->size; i++){
        Connection *sub_con = array_get(node->sub_connections, i);
        render_connection(context, sub_con);
    }

    for (int i = 0 ; i < node->sub_nodes->size; i++){
        Node *sub_node = array_get(node->sub_nodes, i);
        render_node(context, sub_node);
    }

    SDL_Rect screen_rect;
    world_rect_to_screen(&node->close_btn->rect, &screen_rect);

    Button screen_button = *node->close_btn;
    screen_button.rect = screen_rect;

    render_button(context, &screen_button);
    render_pins(context, node);
}

void render_connection_dragging(RenderContext *context) {
    SDL_SetRenderDrawColor(context->renderer, 0, 0, 30, 255);

    render_connection_branch(context, sim_state->new_connection);
    render_connection_points(context, sim_state->new_connection);

    int thickness = (int)(4 * sim_state->camera_zoom);
    for (int t = -thickness / 2; t <= thickness / 2; t++) {
        SDL_RenderDrawLine(context->renderer, sim_state->last_connection_point->x, sim_state->last_connection_point->y + t, sim_state->mouse_x, sim_state->mouse_y + t);
        SDL_RenderDrawLine(context->renderer, sim_state->last_connection_point->x + t, sim_state->last_connection_point->y, sim_state->mouse_x + t, sim_state->mouse_y);
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
        int sx1, sy1;
        world_to_screen(p1->x, p1->y, &sx1, &sy1);

        for (int j = 0; j < p1->neighbors->size; j++) {
            Connection_point *p2 = array_get(p1->neighbors, j);

            if (p1 < p2) {
                int sx2, sy2;
                world_to_screen(p2->x, p2->y, &sx2, &sy2);

                for (int t = -thickness / 2; t <= thickness / 2; t++) {
                    SDL_RenderDrawLine(context->renderer, sx1, sy1 + t, sx2, sy2 + t);
                    SDL_RenderDrawLine(context->renderer, sx1 + t, sy1, sx2 + t, sy2);
                }
            }
        }
    }
}

void render_connection_points(RenderContext *context, Connection *con) {
    for (int i = 0; i < con->points->size; i++) {
        Connection_point *p = array_get(con->points, i);

        int sx, sy;
        world_to_screen(p->x, p->y, &sx, &sy);

        SDL_Rect pin_rect;
        int pin_size = (int)(8 * sim_state->camera_zoom);
        pin_rect.w = pin_size;
        pin_rect.h = pin_size;
        pin_rect.x = sx - pin_size / 2;
        pin_rect.y = sy - pin_size / 2;

        if (sim_state->hovered_connection_point == p) SDL_SetTextureColorMod(context->circle_texture, 255, 255, 60);
        else SDL_SetTextureColorMod(context->circle_texture, 0, 0, 30);

        for (int j = 0; j < sim_state->selected_connection_points->size; j++) {
            Connection_point *current = array_get(sim_state->selected_connection_points, j);
            if (current == p) {
                SDL_SetTextureColorMod(context->circle_texture, 225, 225, 225);
                break;
            }
        }

        SDL_RenderCopy(context->renderer, context->circle_texture, NULL, &pin_rect);
    }
}

void render_selection_box(RenderContext *context) {
    assert(context != NULL);
    assert(sim_state != NULL);

    SDL_Renderer *renderer = context->renderer;
    assert(renderer != NULL);

    SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
    SDL_RenderFillRect(renderer, &sim_state->selection_box);

    SDL_SetRenderDrawColor(renderer, 245, 245, 245, 255);
    SDL_RenderDrawRect(renderer, &sim_state->selection_box);
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

    render_knife_stroke(context);
    render_selection_box(context);

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

    for (int i = 0; i < sim_state->selected_nodes->size; i++) {
        Node *node = array_get(sim_state->selected_nodes, i);
        assert(node != NULL);

        render_selected_node_outline(context, node);
    }

    if (sim_state->is_cable_dragging) render_connection_dragging(context);

    render_top_bar(context);
    for (int i = 0; i < sim_state->buttons->size; i++) {
        Button *button = array_get(sim_state->buttons, i);

        if (strncmp(button->name, "/assets/images/play.png", 23) == 0 && !sim_state->is_paused) { button->name = "/assets/images/pause.png"; }
        if (strncmp(button->name, "/assets/images/pause.png", 24) == 0 && sim_state->is_paused) { button->name = "/assets/images/play.png"; }

        assert(button != NULL);
        render_button(context, button);
    }

    if (sim_state->popup_state != NULL) render_popup(context);

    present_screen(context);
}
