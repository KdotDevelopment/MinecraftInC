#include <gui/font.h>
#include <renderer/textures.h>
#include <renderer/tesselator.h>

#include <util/string.h>
#include <assets/default.h>

#include <stdio.h>
#include <stdint.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

font_t font_create(game_settings_t *settings, char *name, textures_t *textures) {
    font_t font;
    font = (font_t){ 0 };

    font.settings = settings;
    font.texture_name = name;

    int width = 0;
    int *pixels = (int *)0;
    if (strcmp(name, "default.png") == 0) {
        pixels = (int *)asset_default_rgba;
        width = asset_default_width;
    }
    if (pixels == NULL) printf("Failed to load image %s.\n", name);

    for (int i = 0; i < 128; i++) {
        int xt = i % 16;
        int yt = i / 16;
        int x = 0;
        for (uint8_t empty_column = 0; x < 8 && !empty_column; x++) {
            int xPixel = xt * 8 + x;
            empty_column = 1;
            for (int y = 0; y < 8 && empty_column; y++) {
                int yPixel = (yt * 8 + y) * width;
                int pixel = pixels[xPixel + yPixel] & 0xff;
                if (pixel > 128) empty_column = 0;
            }
        }
        
        if (i == 32) { x = 4; }
        font.width_map[i] = x;
    }
    font.texture = textures_load(textures, name);

    return font;
}

static void private_text_render(font_t *font, char *str, int x, int y, uint32_t color, uint8_t darken) {
    if(str != NULL) {
        if(darken) color = (((color >> 8) & 0xfcfcfc) >> 2) << 8;
        glBindTexture(GL_TEXTURE_2D, font->texture);
        tesselator_begin();
        tesselator_color_int(color);
        for(int i = 0, w = 0; i < strlen(str); i++) {
            if(str[i] == 38 && strlen(str) > i + 1) {
                char *hex = string_create("0123456789abcdef");
                int index = string_index_of(hex, str[i + 1]);
                string_free(hex);
                if(index < 0) { index = 15; }
                uint8_t v = (index & 0x08) << 3;
                uint8_t r = ((index & 0x04) >> 2) * 191 + v;
                uint8_t g = ((index & 0x02) >> 1) * 191 + v;
                uint8_t b = (index & 0x01) * 191 + v;
                if(font->settings->anaglyph) {
                    uint8_t ar = (r * 30 + g * 59 + b * 11) / 100;
                    uint8_t ag = (r * 30 + g * 70) / 100;
                    uint8_t ab = (r * 30 + b * 70) / 100;
                    r = ar;
                    g = ag;
                    b = ab;
                }
                color = r << 24 | g << 16 | b << 8;
                i += 2;
                if(darken) { color = (color & 0xfcfcfc) >> 2; }
                tesselator_color_int(color);
            }
            
            int u = str[i] % 16 << 3;
            int v = str[i] / 16 << 3;
            float s = 7.99;
            tesselator_vertex_uv(x + w, y + s, 0.0, u / 128.0, (v + s) / 128.0);
            tesselator_vertex_uv(x + w + s, y + s, 0.0, (u + s) / 128.0, (v + s) / 128.0);
            tesselator_vertex_uv(x + w + s, y, 0.0, (u + s) / 128.0, v / 128.0);
            tesselator_vertex_uv(x + w, y, 0.0, u / 128.0 , v / 128.0);
            w += font->width_map[(int)str[i]];
        }
        tesselator_end();
    }
}

void font_render(font_t *font, char *string, int x, int y, uint32_t color) {
    private_text_render(font, string, x + 1, y + 1, color, 1);
    private_text_render(font, string, x , y , color, 0);
}

void font_render_no_shadow(font_t *font, char *string, int x, int y, uint32_t color) {
    private_text_render(font, string, x , y , color, 0);
}

int font_get_width(font_t *font, char *string) {
    if(string == (char *)0) {
        return 0;
    }

    int width = 0;

    for(int i = 0; i < strlen(string); i++) {
        if(string[i] == '&') i++;
        else width += font->width_map[(int)string[i]];
    }

    return width;
}

char *font_strip_color(char *string) {
    char *new_string = string_create(string);
    for(int i = 0; i < strlen(string); i++) {
        if(string[i] == '&') i++;
        else string_concat(&new_string, (char[]){ string[i], '\0' });
    }

    return new_string;
}