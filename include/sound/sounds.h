#pragma once

#include <sound/sound.h>

#include <util/random.h>

#include <stdint.h>

typedef struct sounds_s {
    uint64_t last_music;
    random_t random;
    sound_t grass[4];
    sound_t gravel[4];
    sound_t stone[4];
    sound_t wood[4];
    sound_t calm[3];
    float music_volume;
    float sound_volume;
    ALCdevice *device;
    ALCcontext *context;
} sounds_t;

sounds_t sounds_create();
void sounds_play_music(sounds_t *sounds, char *music);
void sounds_play_sound(sounds_t *sounds, char *sound, float volume, float pitch);
void sounds_stop_all(sounds_t *sounds);
void sounds_set_music_volume(sounds_t *sounds, float volume);
void sounds_set_sound_volume(sounds_t *sounds, float volume);
void sounds_destroy(sounds_t *sounds);