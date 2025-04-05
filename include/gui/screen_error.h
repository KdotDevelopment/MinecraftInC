#pragma once

#include <gui/screen.h>

screen_t screen_error_create(char *title, char *text);
void screen_error_on_open(struct screen_s *screen);
void screen_error_render(struct screen_s *screen, int mouse_x, int mouse_y);
void screen_error_on_key_pressed(struct screen_s *screen, char event_char, int event_key);