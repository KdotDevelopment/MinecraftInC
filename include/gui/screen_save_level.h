#pragma once

#include <gui/screen.h>

screen_t screen_save_level_create(screen_t *parent);
void screen_save_level_on_open(struct screen_s *screen);
void screen_save_level_open_level(struct screen_s *screen, int level);
void screen_save_level_open_level_from_file(struct screen_s *screen, char *file);