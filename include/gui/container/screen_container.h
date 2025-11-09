#pragma once

#include <gui/screen.h>

screen_t screen_container_create();
void screen_container_render(screen_t *screen, int x, int y, float partial_tick);
void screen_container_on_mouse_clicked(screen_t *screen, int mx, int my, int button);
void screen_container_on_key_pressed(screen_t *screen, char event_char, int event_key);
void screen_container_destroy(screen_t *screen);