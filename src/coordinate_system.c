#include "coordinate_system.h"

SDL_Rect float_rect_to_sdl_rect(Float_Rect *rect) {
    return (SDL_Rect){(int)rect->x, (int)rect->y, (int)rect->w, (int)rect->h};
}

Float_Rect sdl_rect_to_float_rect(SDL_Rect *rect) {
    return (Float_Rect){(float)rect->x, (float)rect->y, (float)rect->w, (float)rect->h};
}

void point_print(SDL_Point *p) {
    printf("(%d, %d)\n", p->x, p->y);
}

int point_cross(SDL_Point *a, SDL_Point *b) {
    return a->x * b->y - a->y * b->x;
}

SDL_Point point_add(SDL_Point *a, SDL_Point *b) {
    SDL_Point res = {a->x + b->x, a->y + b->y};
    return res;
}

SDL_Point point_subtract(SDL_Point *a, SDL_Point *b) {
    SDL_Point res = {a->x - b->x, a->y - b->y};
    return res;
}

SDL_Point point_multiply(SDL_Point *a, double k) {
    SDL_Point res = {(int)(a->x * k), (int)(a->y * k)};
    return res;
}

SDL_Point point_divide(SDL_Point *a, double k) {
    SDL_Point res = {(int)(a->x / k), (int)(a->y / k)};
    return res;
}

int point_orient(SDL_Point *a, SDL_Point *b, SDL_Point *c) {
    SDL_Point ab = point_subtract(b, a);
    SDL_Point ac = point_subtract(c, a);
    return point_cross(&ab, &ac);
}

// check if two segments lines intersect
int segment_intersection(SDL_Point *a, SDL_Point *b, SDL_Point *c, SDL_Point *d, SDL_Point *out) {
    double oa = point_orient(c, d, a);
    double ob = point_orient(c, d, b);
    double oc = point_orient(a, b, c);
    double od = point_orient(a, b, d);

    // Proper intersection exists iff signs are opposite
    if (oa * ob < 0 && oc * od < 0) {
        double t = oa / (oa - ob);
        SDL_Point diff = point_subtract(b, a);
        SDL_Point scaled = point_multiply(&diff, t);
        *out = point_add(a, &scaled);
        return 1;
    }
    return 0;
}

float distance_to_line_segment(float px, float py, float x1, float y1, float x2, float y2) {
    float A = px - x1;
    float B = py - y1;
    float C = x2 - x1;
    float D = y2 - y1;

    float dot = A * C + B * D;
    float len_sq = C * C + D * D;

    if (len_sq == 0) {
        return sqrt(A * A + B * B);
    }

    float param = dot / len_sq;

    float xx, yy;
    if (param < 0) {
        xx = x1;
        yy = y1;
    } else if (param > 1) {
        xx = x2;
        yy = y2;
    } else {
        xx = x1 + param * C;
        yy = y1 + param * D;
    }

    float dx = px - xx;
    float dy = py - yy;
    return sqrt(dx * dx + dy * dy);
}
