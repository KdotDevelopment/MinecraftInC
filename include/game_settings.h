#pragma once

#include <stdint.h>

typedef struct {
    char *name;
    int key;
} key_binding_t;

typedef struct {
    uint8_t music;
    uint8_t sound;
    uint8_t invert_mouse;
    uint8_t show_framerate;
    int view_distance;
    uint8_t show_bobbing;
    uint8_t anaglyph;
    uint8_t limit_framerate;
    struct minecraft_s *minecraft;
    int settings_count;
    key_binding_t forward_key;
    key_binding_t left_key;
    key_binding_t back_key;
    key_binding_t right_key;
    key_binding_t jump_key;
    key_binding_t build_key;
    key_binding_t chat_key;
    key_binding_t toggle_fog_key;
    key_binding_t save_location_key;
    key_binding_t load_location_key;
    key_binding_t **bindings; //arraylist
} game_settings_t;

void game_settings_create(game_settings_t *settings, struct minecraft_s *minecraft);
void game_settings_toggle_setting(game_settings_t *settings, int setting);
char *game_settings_get_binding(game_settings_t *settings, int binding);
void game_settings_set_binding(game_settings_t *settings, int binding, int key);
char *game_settings_get_setting(game_settings_t *settings, int setting);