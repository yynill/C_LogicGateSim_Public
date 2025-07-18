#ifndef RENDERER_H
#define RENDERER_H
#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "main.h"
#include "simulation.h"

#define TOP_BAR_HEIGHT 48

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT ((800) + TOP_BAR_HEIGHT)

#define NODE_WIDTH 78
#define SMALL_NODE_WIDTH 54
#define NODE_HEIGHT 36
#define PIN_SIZE 10

#define BUTTON_WIDTH 64
#define BUTTON_HEIGHT 32

#define PADDING 10

#define IMG_PATH_PLAY "/assets/images/play.png"
#define IMG_PATH_PAUSE "/assets/images/pause.png"
#define IMG_PATH_STEP_BACK "/assets/images/step_back.png"
#define IMG_PATH_STEP_FORTH "/assets/images/step_forth.png"
#define IMG_PATH_PLUS "/assets/images/plus.png"
#define IMG_PATH_MINUS "/assets/images/minus.png"
#define IMG_PATH_RELOAD "/assets/images/reload.png"
#define IMG_PATH_DOWNLOAD "/assets/images/download.png"
#define IMG_PATH_TRASH "/assets/images/trash.png"
#define IMG_PATH_SWITCH_ON "/assets/images/switch_on.png"
#define IMG_PATH_SWITCH_OFF "/assets/images/switch_off.png"
#define IMG_PATH_LIGHT_ON "/assets/images/light_on.png"
#define IMG_PATH_LIGHT_OFF "/assets/images/light_off.png"
#define IMG_PATH_ARROWS "/assets/images/4_arrows.png"
#define IMG_PATH_CIRCLE "/assets/images/circle.png"

typedef struct SimulationState SimulationState;
typedef struct Node Node;
typedef struct Connection Connection;

typedef struct {
    SDL_Texture *play_texture;
    SDL_Texture *pause_texture;
    SDL_Texture *step_back_texture;
    SDL_Texture *step_forth_texture;
    SDL_Texture *plus_texture;
    SDL_Texture *minus_texture;
    SDL_Texture *reload_texture;
    SDL_Texture *download_texture;
    SDL_Texture *trash_texture;
    SDL_Texture *switch_on_texture;
    SDL_Texture *switch_off_texture;
    SDL_Texture *light_on_texture;
    SDL_Texture *light_off_texture;
    SDL_Texture *arrows_texture;
    SDL_Texture *circle_texture;
} ImageCache;

typedef struct {
    SDL_Texture *texture;
    SDL_Rect *rect;
} TextChacheElement;

typedef struct {
    TextChacheElement *not_texture;
    TextChacheElement *and_texture;
    TextChacheElement *or_texture;
    TextChacheElement *xor_texture;
    TextChacheElement *nor_texture;
    TextChacheElement *xnor_texture;
    TextChacheElement *nand_texture;
    TextChacheElement *switch_texture;
    TextChacheElement *light_texture;
    TextChacheElement *note_texture;
} TextCache;

typedef struct
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    ImageCache image_cache;
    TextCache text_cache;
    // DynamicArray *other_text_cache;
} RenderContext;

RenderContext *init_renderer();
void clear_screen(RenderContext *context);
void present_screen(RenderContext *context);
void cleanup_renderer(RenderContext *context);
TextChacheElement *text_to_texture(RenderContext *context, char *text, SDL_Color *color);
void render_text(RenderContext *context, char *text, int x, int y, SDL_Color *color, float zoom);
void render_img(RenderContext *context, SDL_Texture *texture, SDL_Rect *rect);
void render_popup(RenderContext *context);
void render_connection(RenderContext *context, Connection *con);
void render_connection_branch(RenderContext *context, Connection *con);
void render_connection_points(RenderContext *context, Connection *con);
void render_single_node(RenderContext *context, Node *node);
void render_pins(RenderContext *context, Node *node);
void render(RenderContext *context);

void world_to_screen(float world_x, float world_y, int *screen_x, int *screen_y);
void screen_to_world(int screen_x, int screen_y, float *world_x, float *world_y);
void screen_rect_to_world(const SDL_Rect *screen, SDL_Rect *out_world);

#endif // RENDERER_H
