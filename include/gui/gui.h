#pragma once

#include <gui/font.h>

#include <stdint.h>

void gui_fill(int x0, int y0, int x1, int y1, uint32_t color);
void gui_fill_gradient(int x0, int y0, int x1, int y1, uint32_t color0, uint32_t color1);
void gui_draw_centered_string(font_t *font, char *string, int x, int y, uint32_t color);
void gui_draw_string(font_t *font, char *string, int x, int y, uint32_t color);
void gui_blit(int x0, int y0, int u, int v, int width, int height, float blit_offset);