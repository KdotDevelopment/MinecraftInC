#pragma once

#include <game_settings.h>
#include <renderer/textures.h>

typedef struct {
    int width_map[256];
    char *texture_name;
    int texture;
    game_settings_t *settings;
} font_t;

font_t font_create(game_settings_t *settings, char *name, textures_t *textures);
void font_render(font_t *font, char *string, int x, int y, uint32_t color);
void font_render_no_shadow(font_t *font, char *string, int x, int y, uint32_t color);
int font_get_width(font_t *font, char *string);
char *font_strip_color(char *string);
