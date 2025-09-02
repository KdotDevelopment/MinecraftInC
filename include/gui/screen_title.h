#pragma once

#include <gui/screen.h>

screen_t screen_title_create();
void screen_title_tick(screen_t *screen);
void screen_title_on_open(screen_t *screen);
void screen_title_on_button_clicked(screen_t *screen, button_t *button);
void screen_title_render(screen_t *screen, int mouse_x, int mouse_y, float partial_tick);