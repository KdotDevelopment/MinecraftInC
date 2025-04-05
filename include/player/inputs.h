#pragma once

#include <game_settings.h>

#include <stdint.h>

typedef struct {
    uint8_t key_states[10];
    game_settings_t *settings;
    float x;
    float z;
    uint8_t jumping;
} inputs_t;

inputs_t inputs_create(game_settings_t *settings);
void inputs_set_key_state(inputs_t *inputs, uint8_t key, uint8_t state);
void inputs_reset_keys(inputs_t *inputs);
void inputs_update_movement(inputs_t *inputs);