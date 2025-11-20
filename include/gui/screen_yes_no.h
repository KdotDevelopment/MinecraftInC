#pragma once

#include <gui/screen.h>

screen_t screen_yes_no_create(screen_t *parent, char *message1, char *message2, int world_id);
void screen_yes_no_on_open(screen_t *screen);
void screen_yes_no_on_button_clicked(screen_t *screen, button_t *button);
void screen_yes_no_render(screen_t *screen, int mouse_x, int mouse_y, float partial_tick);