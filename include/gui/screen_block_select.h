#pragma once

#include <gui/screen.h>

screen_t screen_block_select_create();
void screen_block_select_render(struct screen_s *screen, int mx, int my);
void screen_block_select_on_mouse_clicked(struct screen_s *proto_screen, int x, int y, int button);