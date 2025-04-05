#include <level/block/block_sound.h>

#include <stdlib.h>

struct block_sounds_s block_sounds = {
    .none =   { .volume = 0.0, .pitch = 0.0, .type = BLOCK_SOUND_NONE, .name = "-"},      
    .grass =  { .volume = 0.6, .pitch = 1.0, .type = BLOCK_SOUND_GRASS, .name = "Grass" }, 
    .wool =   { .volume = 0.7, .pitch = 1.2, .type = BLOCK_SOUND_WOOL, .name = "Grass" },  
    .gravel = { .volume = 1.0, .pitch = 1.0, .type = BLOCK_SOUND_GRAVEL, .name = "Gravel" },
    .stone =  { .volume = 1.0, .pitch = 1.0, .type = BLOCK_SOUND_STONE, .name = "Stone" }, 
    .metal =  { .volume = 1.0, .pitch = 2.0, .type = BLOCK_SOUND_METAL, .name = "Stone" }, 
    .wood =   { .volume = 1.0, .pitch = 1.0, .type = BLOCK_SOUND_WOOD, .name = "Wood" },  
};

float block_sound_get_volume(block_sound_t block_sound) {
    return block_sound.volume / ((rand() / RAND_MAX) * 0.4 + 1) * 0.5;
}

float block_sound_get_pitch(block_sound_t block_sound) {
    return block_sound.pitch / ((rand() / RAND_MAX) * 0.2 + 0.9);
}