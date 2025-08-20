#include <world/block/block_sound.h>

#include <sound/sounds.h>

#include <stdlib.h>

struct block_sounds_s block_sounds = {
    .none =   { .volume = 1.0, .pitch = 1.0, .type = BLOCK_SOUND_NONE, .base_type = SOUND_STEP_STONE},      
    .grass =  { .volume = 1.0, .pitch = 1.0, .type = BLOCK_SOUND_GRASS, .base_type = SOUND_STEP_GRASS},
    .wool =   { .volume = 1.0, .pitch = 1.0, .type = BLOCK_SOUND_CLOTH, .base_type = SOUND_STEP_CLOTH},
    .gravel = { .volume = 1.0, .pitch = 1.0, .type = BLOCK_SOUND_GRAVEL, .base_type = SOUND_STEP_GRAVEL},
    .stone =  { .volume = 1.0, .pitch = 1.0, .type = BLOCK_SOUND_STONE, .base_type = SOUND_STEP_STONE},
    .metal =  { .volume = 1.0, .pitch = 1.5, .type = BLOCK_SOUND_METAL, .base_type = SOUND_STEP_STONE},
    .wood =   { .volume = 1.0, .pitch = 1.0, .type = BLOCK_SOUND_WOOD, .base_type = SOUND_STEP_WOOD},
    .glass =  { .volume = 1.0, .pitch = 1.0, .type = BLOCK_SOUND_GLASS, .base_type = SOUND_STEP_STONE },
    .sand =   { .volume = 1.0, .pitch = 1.0, .type = BLOCK_SOUND_SAND, .base_type = SOUND_STEP_SAND }
};

float block_sound_get_volume(block_sound_t block_sound) {
    return block_sound.volume / ((rand() / RAND_MAX) * 0.4 + 1) * 0.5;
}

float block_sound_get_pitch(block_sound_t block_sound) {
    return block_sound.pitch / ((rand() / RAND_MAX) * 0.2 + 0.9);
}