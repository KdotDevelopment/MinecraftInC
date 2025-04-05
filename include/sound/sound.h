#pragma once

#define AL_LIBTYPE_STATIC

#include <AL/al.h>
#include <AL/alc.h>
#include <vorbis/vorbisfile.h>

#define AL_LIBTYPE_STATIC

typedef struct sound_s {
    ALuint buffer;
    ALuint source;
    ALenum format;
    ALsizei sample_rate;
} sound_t;

sound_t sound_load(const unsigned char *data, size_t size);
void sound_play(sound_t *sound, float volume, float pitch);
void sound_stop(sound_t *sound);
void sound_destroy(sound_t *sound);