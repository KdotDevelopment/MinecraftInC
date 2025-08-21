#pragma once

#include <stdint.h>

typedef enum {
    BLOCK_SOUND_NONE,
    BLOCK_SOUND_GRASS,
    BLOCK_SOUND_CLOTH,
    BLOCK_SOUND_GRAVEL,
    BLOCK_SOUND_STONE,
    BLOCK_SOUND_METAL,
    BLOCK_SOUND_WOOD,
    BLOCK_SOUND_GLASS,
    BLOCK_SOUND_SAND,
} block_sound_type_e;

typedef struct {
    float volume;
    float pitch;
    block_sound_type_e type;
    uint8_t base_type;
} block_sound_t;

// Global struct
extern struct block_sounds_s {
    block_sound_t none;
    block_sound_t grass;
    block_sound_t wool;
    block_sound_t gravel;
    block_sound_t stone;
    block_sound_t metal;
    block_sound_t wood;
    block_sound_t glass;
    block_sound_t sand;
} block_sounds;

float block_sound_get_volume(block_sound_t block_sound);
float block_sound_get_pitch(block_sound_t block_sound);