#pragma once

#include <sound/sound.h>

#include <util/random.h>

#include <stdint.h>

enum {
    SOUND_NONE,
    SOUND_FIRE_FIRE,
    SOUND_FIRE_IGNITE,

    SOUND_LIQUID_LAVA,
    SOUND_LIQUID_WATER,

    SOUND_MOB_PIG,    // 3
    SOUND_MOB_PIG_DEATH,
    SOUND_MOB_SHEEP,  // 3

    SOUND_RANDOM_BOW,
    SOUND_RANDOM_CLICK,
    SOUND_RANDOM_DRR, // arrow hit
    SOUND_RANDOM_EXPLODE,
    SOUND_RANDOM_FIZZ,
    SOUND_RANDOM_FUSE,
    SOUND_RANDOM_GLASS, // 3
    SOUND_RANDOM_HURT,
    SOUND_RANDOM_POP,
    SOUND_RANDOM_SPLASH,

    SOUND_STEP_CLOTH, // 4
    SOUND_STEP_GRASS, // 4
    SOUND_STEP_GRAVEL,// 4
    SOUND_STEP_SAND,  // 4
    SOUND_STEP_STONE, // 4
    SOUND_STEP_WOOD,  // 4
};

typedef struct sounds_s {
    uint64_t last_music;
    random_t random;
    sound_t grass[4];
    sound_t gravel[4];
    sound_t stone[4];
    sound_t wood[4];
    sound_t sand[4];
    sound_t wool[4];
    sound_t bow;
    sound_t click;
    sound_t drr;
    sound_t explode;
    sound_t fizz;
    sound_t fuse;
    sound_t glass[3];
    sound_t hurt;
    sound_t pop;
    sound_t splash;
    sound_t pig[3];
    sound_t pig_death;
    sound_t sheep[3];
    sound_t lava;
    sound_t water;
    sound_t fire;
    sound_t ignite;
    float music_volume;
    float sound_volume;
    ALCdevice *device;
    ALCcontext *context;
} sounds_t;

sounds_t sounds_create();
void sounds_play_music(sounds_t *sounds, char *music);
void sounds_play_sound_at(sounds_t *sounds, uint8_t sound, float x, float y, float z, float volume, float pitch);
void sounds_play_sound(sounds_t *sounds, uint8_t sound, float volume, float pitch);
void sounds_stop_all(sounds_t *sounds);
void sounds_set_music_volume(sounds_t *sounds, float volume);
void sounds_set_sound_volume(sounds_t *sounds, float volume);
void sounds_destroy(sounds_t *sounds);