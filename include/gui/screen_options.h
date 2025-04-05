#pragma once

#include <gui/screen.h>

screen_t screen_options_create(screen_t *parent, game_settings_t *settings);
void screen_options_on_open(struct screen_s *screen);
void screen_options_on_button_clicked(struct screen_s *screen, button_t *button);
void screen_options_render(struct screen_s *screen, int mouse_x, int mouse_y);