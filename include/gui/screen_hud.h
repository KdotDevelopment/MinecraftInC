#pragma once

#include <util/random.h>

struct minecraft_s;

typedef struct {
    //chat line
    random_t random;
    struct minecraft_s *minecraft;
    int width;
    int height;
    //const char *hovered_player;
    int ticks;
} screen_hud_t;

screen_hud_t screen_hud_create(struct minecraft_s *minecraft, int width, int height);
void screen_hud_render(screen_hud_t *hud, float delta, float mx, float my);
void screen_hud_destroy(screen_hud_t *hud);