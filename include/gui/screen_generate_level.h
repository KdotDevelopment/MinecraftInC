#pragma once

#include <gui/screen.h>

screen_t screen_generate_level_create(screen_t *parent);
void screen_generate_level_on_open(struct screen_s *screen);
void screen_generate_level_on_button_clicked(struct screen_s *screen, button_t *button);
void screen_generate_level_render(struct screen_s *screen, int mouse_x, int mouse_y);