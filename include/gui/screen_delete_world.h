#pragma once

#include <gui/screen.h>

screen_t screen_delete_world_create(screen_t *parent);
void screen_delete_world_on_open(screen_t *screen);
void screen_delete_world_select(screen_t *screen, int world_id);
void screen_delete_world_on_button_clicked(screen_t *screen, button_t *button);
void screen_delete_world_render(screen_t *screen, int mouse_x, int mouse_y, float partial_tick);
void screen_delete_world_delete(screen_t *screen, uint8_t perform, int world_id);