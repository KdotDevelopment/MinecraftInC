#pragma once

#include <game_settings.h>
#include <renderer/texture/texture_animated.h>

#include <stdint.h>

typedef struct {
    uint32_t *textures; //arraylist
    char **texture_names; //arraylist
    uint8_t *texture_buffer;
    uint32_t id_buffer;
    texture_animated_t **animations; //arraylist
    game_settings_t *settings;
} textures_t;

textures_t textures_create(game_settings_t *settings);
int textures_load(textures_t *textures, const char *resource);
void textures_reload(textures_t *textures);
void textures_register_animation(textures_t *textures, texture_animated_t *texture);
void textures_destroy(textures_t *textures);