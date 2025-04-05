#pragma once

#include <gui/screen.h>

screen_t screen_pause_create();
void screen_pause_on_open(struct screen_s *screen);
void screen_pause_on_button_clicked(struct screen_s *screen, button_t *button);
void screen_pause_render(struct screen_s *screen, int mouse_x, int mouse_y);