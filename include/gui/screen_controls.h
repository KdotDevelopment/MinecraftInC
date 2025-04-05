#pragma once

#include <gui/screen.h>
#include <game_settings.h>

screen_t screen_controls_create(screen_t *parent, game_settings_t *settings);
void screen_controls_on_open(struct screen_s *screen);
void screen_controls_render(struct screen_s *screen, int mx, int my);
void screen_controls_on_key_pressed(struct screen_s *screen, char event_char, int event_key);
void screen_controls_on_button_clicked(struct screen_s *proto_screen, button_t *button);