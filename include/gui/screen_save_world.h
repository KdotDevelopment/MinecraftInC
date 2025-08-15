#pragma once

#include <gui/screen.h>

screen_t screen_save_world_create(screen_t *parent);
void screen_save_world_on_open(struct screen_s *screen);
void screen_save_world_open_world(struct screen_s *screen, int world);
void screen_save_world_open_world_from_file(struct screen_s *screen, char *file);