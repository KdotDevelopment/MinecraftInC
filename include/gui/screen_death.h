#pragma once

#include <gui/screen.h>

screen_t screen_death_create();
void screen_death_on_button_clicked(struct screen_s *screen, button_t *button);
void screen_death_on_open(struct screen_s *screen);
void screen_death_render(struct screen_s *screen, int mx, int my);