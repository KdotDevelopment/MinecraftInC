#pragma once

typedef enum {
    BLOCK_SOUND_NONE,
    BLOCK_SOUND_GRASS,
    BLOCK_SOUND_WOOL,
    BLOCK_SOUND_GRAVEL,
    BLOCK_SOUND_STONE,
    BLOCK_SOUND_METAL,
    BLOCK_SOUND_WOOD,
} block_sound_type_t;

typedef struct {
    float volume;
    float pitch;
    char *name;
    block_sound_type_t type;
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
} block_sounds;

float block_sound_get_volume(block_sound_t block_sound);
float block_sound_get_pitch(block_sound_t block_sound);