#pragma once

#include <gui/screen.h>

screen_t screen_chat_input_create();
void screen_chat_input_tick(struct screen_s *screen);
void screen_chat_input_render(struct screen_s *screen, int mx, int my);
void screen_chat_input_on_key_pressed(struct screen_s *screen, char event_char, int event_key);
void screen_chat_input_on_mouse_clicked(struct screen_s *screen, int x, int y, int button);
void screen_chat_input_destroy(struct screen_s *screen);