#pragma once

#include <gui/screen.h>

screen_t screen_level_name_create(screen_t *parent, char *name, int id);
void screen_level_name_on_open(struct screen_s *screen);
void screen_level_name_on_close(struct screen_s *screen);
void screen_level_name_tick(struct screen_s *screen);
void screen_level_name_render(struct screen_s *screen, int mouse_x, int mouse_y);
void screen_level_name_on_key_pressed(struct screen_s *screen, char event_char, int event_key);
void screen_level_name_on_button_clicked(struct screen_s *screen, button_t *button);
void screen_level_name_destroy(struct screen_s *screen);