#define AL_LIBTYPE_STATIC

#include <sound/sounds.h>

#include "assets/music/calm1.h"
#include "assets/music/calm2.h"
#include "assets/music/calm3.h"
#include "assets/sound/step/grass1.h"
#include "assets/sound/step/grass2.h"
#include "assets/sound/step/grass3.h"
#include "assets/sound/step/grass4.h"
#include "assets/sound/step/gravel1.h"
#include "assets/sound/step/gravel2.h"
#include "assets/sound/step/gravel3.h"
#include "assets/sound/step/gravel4.h"
#include "assets/sound/step/stone1.h"
#include "assets/sound/step/stone2.h"
#include "assets/sound/step/stone3.h"
#include "assets/sound/step/stone4.h"
#include "assets/sound/step/wood1.h"
#include "assets/sound/step/wood2.h"
#include "assets/sound/step/wood3.h"
#include "assets/sound/step/wood4.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

// Initialize OpenAL
void sounds_init(sounds_t *sounds) {
    sounds->device = alcOpenDevice(NULL); // Open default device
    if(!sounds->device) {
        fprintf(stderr, "Failed to open OpenAL device.\n");
        exit(0);
    }

    sounds->context = alcCreateContext(sounds->device, NULL);
    if(!sounds->context || !alcMakeContextCurrent(sounds->context)) {
        fprintf(stderr, "Failed to create or set OpenAL context.\n");
        if(sounds->context) alcDestroyContext(sounds->context);
        alcCloseDevice(sounds->device);
        exit(0);
    }
}

void sounds_stop_all(sounds_t *sounds) {
    for(int i = 0; i < 3; i++) {
        sound_stop(&sounds->calm[i]);
    }
    for(int i = 0; i < 4; i++) {
        sound_stop(&sounds->grass[i]);
        sound_stop(&sounds->gravel[i]);
        sound_stop(&sounds->stone[i]);
        sound_stop(&sounds->wood[i]);
    }
}

void sounds_set_music_volume(sounds_t *sounds, float volume) {
    for(int i = 0; i < 3; i++) {
        alSourcef(sounds->calm[i].source, AL_GAIN, volume * 0.4);
    }
    sounds->music_volume = volume;
}

void sounds_set_sound_volume(sounds_t *sounds, float volume) {
    for(int i = 0; i < 4; i++) {
        alSourcef(sounds->grass[i].source, AL_GAIN, volume);
        alSourcef(sounds->gravel[i].source, AL_GAIN, volume);
        alSourcef(sounds->stone[i].source, AL_GAIN, volume);
        alSourcef(sounds->wood[i].source, AL_GAIN, volume);
    }
    sounds->sound_volume = volume;
}

sounds_t sounds_create() {
    sounds_t sounds = {0};

    sounds.random = random_create(time(NULL));
    sounds_init(&sounds);

    sounds.calm[0] = sound_load(asset_calm1_ogg, sizeof(asset_calm1_ogg));
    sounds.calm[1] = sound_load(asset_calm2_ogg, sizeof(asset_calm2_ogg));
    sounds.calm[2] = sound_load(asset_calm3_ogg, sizeof(asset_calm3_ogg));
    sounds.grass[0] = sound_load(asset_grass1_ogg, sizeof(asset_grass1_ogg));
    sounds.grass[1] = sound_load(asset_grass2_ogg, sizeof(asset_grass2_ogg));
    sounds.grass[2] = sound_load(asset_grass3_ogg, sizeof(asset_grass3_ogg));
    sounds.grass[3] = sound_load(asset_grass4_ogg, sizeof(asset_grass4_ogg));
    sounds.gravel[0] = sound_load(asset_gravel1_ogg, sizeof(asset_gravel1_ogg));
    sounds.gravel[1] = sound_load(asset_gravel2_ogg, sizeof(asset_gravel2_ogg));
    sounds.gravel[2] = sound_load(asset_gravel3_ogg, sizeof(asset_gravel3_ogg));
    sounds.gravel[3] = sound_load(asset_gravel4_ogg, sizeof(asset_gravel4_ogg));
    sounds.stone[0] = sound_load(asset_stone1_ogg, sizeof(asset_stone1_ogg));
    sounds.stone[1] = sound_load(asset_stone2_ogg, sizeof(asset_stone2_ogg));
    sounds.stone[2] = sound_load(asset_stone3_ogg, sizeof(asset_stone3_ogg));
    sounds.stone[3] = sound_load(asset_stone4_ogg, sizeof(asset_stone4_ogg));
    sounds.wood[0] = sound_load(asset_wood1_ogg, sizeof(asset_wood1_ogg));
    sounds.wood[1] = sound_load(asset_wood2_ogg, sizeof(asset_wood2_ogg));
    sounds.wood[2] = sound_load(asset_wood3_ogg, sizeof(asset_wood3_ogg));
    sounds.wood[3] = sound_load(asset_wood4_ogg, sizeof(asset_wood4_ogg));

    sounds.music_volume = 1.0f;
    sounds.sound_volume = 1.0f;

    for (int i = 0; i < 3; i++) {
        alSource3f(sounds.calm[i].source, AL_POSITION, 0, 0, 0);
    }

    for (int i = 0; i < 4; i++) {
        alSource3f(sounds.grass[i].source, AL_POSITION, 0, 0, 0);
        alSource3f(sounds.gravel[i].source, AL_POSITION, 0, 0, 0);
        alSource3f(sounds.stone[i].source, AL_POSITION, 0, 0, 0);
        alSource3f(sounds.wood[i].source, AL_POSITION, 0, 0, 0);
    }

    return sounds;
}

void sounds_play_music(sounds_t *sounds, char *music) {
    if(strcmp(music, "Calm") == 0) {
        sounds_stop_all(sounds);
        sound_play(&sounds->calm[random_next_int_range(&sounds->random, 0, 2)], 0.2 * sounds->music_volume, 1.0f);
    }
}

void sounds_play_sound(sounds_t *sounds, char *sound, float volume, float pitch) {
    if(strcmp(sound, "Grass") == 0) {
        sound_play(&sounds->grass[random_next_int_range(&sounds->random, 0, 3)], volume * 0.2 * sounds->sound_volume, pitch);
    }
    if(strcmp(sound, "Gravel") == 0) {
        sound_play(&sounds->gravel[random_next_int_range(&sounds->random, 0, 3)], volume * 0.2 * sounds->sound_volume, pitch);
    }
    if(strcmp(sound, "Stone") == 0) {
        sound_play(&sounds->stone[random_next_int_range(&sounds->random, 0, 3)], volume * 0.2 * sounds->sound_volume, pitch);
    }
    if(strcmp(sound, "Wood") == 0) {
        sound_play(&sounds->wood[random_next_int_range(&sounds->random, 0, 3)], volume * 0.2 * sounds->sound_volume, pitch);
    }
}

void sounds_destroy(sounds_t *sounds) {
    for (int i = 0; i < 3; i++) {
        sound_destroy(&sounds->calm[i]);
    }

    for (int i = 0; i < 4; i++) {
        sound_destroy(&sounds->grass[i]);
        sound_destroy(&sounds->gravel[i]);
        sound_destroy(&sounds->stone[i]);
        sound_destroy(&sounds->wood[i]);
    }
    alcMakeContextCurrent(NULL);
    alcDestroyContext(sounds->context);
    alcCloseDevice(sounds->device);
}