#pragma once

#include <gui/screen.h>

screen_t screen_select_world_create(screen_t *parent);
void screen_select_world_on_open(screen_t *screen);
void screen_select_world_on_button_clicked(screen_t *screen, button_t *button);
void screen_select_world_render(screen_t *screen, int mouse_x, int mouse_y, float partial_tick);
