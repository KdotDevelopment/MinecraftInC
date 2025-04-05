#pragma once

#include <gui/screen.h>

screen_t screen_load_level_create(screen_t *parent);
void screen_load_level_on_open(struct screen_s *screen);
void screen_load_level_on_button_clicked(struct screen_s *screen, button_t *button);
void screen_load_level_open_level(struct screen_s *screen, int level);
void screen_load_level_open_level_from_file(struct screen_s *screen, char *file);
void screen_load_level_render(struct screen_s *screen, int mouse_x, int mouse_y);