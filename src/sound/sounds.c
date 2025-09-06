#define AL_LIBTYPE_STATIC

#include <sound/sounds.h>

#include "assets/sound/fire/fire.h"
#include "assets/sound/fire/ignite.h"

#include "assets/sound/liquid/lava.h"
#include "assets/sound/liquid/water.h"

#include "assets/sound/mob/pig1.h"
#include "assets/sound/mob/pig2.h"
#include "assets/sound/mob/pig3.h"
#include "assets/sound/mob/pigdeath.h"
#include "assets/sound/mob/sheep1.h"
#include "assets/sound/mob/sheep2.h"
#include "assets/sound/mob/sheep3.h"

#include "assets/sound/random/bow.h"
#include "assets/sound/random/click.h"
#include "assets/sound/random/drr.h"
#include "assets/sound/random/explode.h"
#include "assets/sound/random/fizz.h"
#include "assets/sound/random/fuse.h"
#include "assets/sound/random/glass1.h"
#include "assets/sound/random/glass2.h"
#include "assets/sound/random/glass3.h"
#include "assets/sound/random/hurt.h"
#include "assets/sound/random/pop.h"
#include "assets/sound/random/splash.h"

#include "assets/sound/step/cloth1.h"
#include "assets/sound/step/cloth2.h"
#include "assets/sound/step/cloth3.h"
#include "assets/sound/step/cloth4.h"
#include "assets/sound/step/grass1.h"
#include "assets/sound/step/grass2.h"
#include "assets/sound/step/grass3.h"
#include "assets/sound/step/grass4.h"
#include "assets/sound/step/gravel1.h"
#include "assets/sound/step/gravel2.h"
#include "assets/sound/step/gravel3.h"
#include "assets/sound/step/gravel4.h"
#include "assets/sound/step/sand1.h"
#include "assets/sound/step/sand2.h"
#include "assets/sound/step/sand3.h"
#include "assets/sound/step/sand4.h"
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
    for(int i = 0; i < 4; i++) {
        sound_stop(&sounds->grass[i]);
        sound_stop(&sounds->gravel[i]);
        sound_stop(&sounds->stone[i]);
        sound_stop(&sounds->wood[i]);
    }
    for(int i = 0; i < 3; i++) {
        sound_stop(&sounds->sheep[i]);
        sound_stop(&sounds->pig[i]);
        sound_stop(&sounds->glass[i]);
    }
    sound_stop(&sounds->bow);
    sound_stop(&sounds->click);
    sound_stop(&sounds->drr);
    sound_stop(&sounds->explode);
    sound_stop(&sounds->fizz);
    sound_stop(&sounds->fuse);
    sound_stop(&sounds->hurt);
    sound_stop(&sounds->pop);
    sound_stop(&sounds->splash);
    sound_stop(&sounds->pig_death);
    sound_stop(&sounds->lava);
    sound_stop(&sounds->water);
    sound_stop(&sounds->fire);
    sound_stop(&sounds->ignite);
}

void sounds_set_music_volume(sounds_t *sounds, float volume) {
    // Music disabled in this version
}

void sounds_set_sound_volume(sounds_t *sounds, float volume) {
    for(int i = 0; i < 4; i++) {
        alSourcef(sounds->grass[i].source, AL_GAIN, volume);
        alSourcef(sounds->gravel[i].source, AL_GAIN, volume);
        alSourcef(sounds->stone[i].source, AL_GAIN, volume);
        alSourcef(sounds->wood[i].source, AL_GAIN, volume);
    }
    for(int i = 0; i < 3; i++) {
        alSourcef(sounds->sheep[i].source, AL_GAIN, volume);
        alSourcef(sounds->pig[i].source, AL_GAIN, volume);
        alSourcef(sounds->glass[i].source, AL_GAIN, volume);
    }
    alSourcef(sounds->bow.source, AL_GAIN, volume);
    alSourcef(sounds->click.source, AL_GAIN, volume);
    alSourcef(sounds->drr.source, AL_GAIN, volume);
    alSourcef(sounds->explode.source, AL_GAIN, volume);
    alSourcef(sounds->fizz.source, AL_GAIN, volume);
    alSourcef(sounds->fuse.source, AL_GAIN, volume);
    alSourcef(sounds->hurt.source, AL_GAIN, volume);
    alSourcef(sounds->pop.source, AL_GAIN, volume);
    alSourcef(sounds->splash.source, AL_GAIN, volume);
    alSourcef(sounds->pig_death.source, AL_GAIN, volume);
    alSourcef(sounds->lava.source, AL_GAIN, volume);
    alSourcef(sounds->water.source, AL_GAIN, volume);
    alSourcef(sounds->fire.source, AL_GAIN, volume);
    alSourcef(sounds->ignite.source, AL_GAIN, volume);

    sounds->sound_volume = volume;
}

sounds_t sounds_create() {
    sounds_t sounds = {0};

    sounds.random = random_create(time(NULL));
    sounds_init(&sounds);

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
    sounds.sheep[0] = sound_load(asset_sheep1_ogg, sizeof(asset_sheep1_ogg));
    sounds.sheep[1] = sound_load(asset_sheep2_ogg, sizeof(asset_sheep2_ogg));
    sounds.sheep[2] = sound_load(asset_sheep3_ogg, sizeof(asset_sheep3_ogg));
    sounds.pig[0] = sound_load(asset_pig1_ogg, sizeof(asset_pig1_ogg));
    sounds.pig[1] = sound_load(asset_pig2_ogg, sizeof(asset_pig2_ogg));
    sounds.pig[2] = sound_load(asset_pig3_ogg, sizeof(asset_pig3_ogg));
    sounds.pig_death = sound_load(asset_pigdeath_ogg, sizeof(asset_pigdeath_ogg));
    sounds.bow = sound_load(asset_bow_ogg, sizeof(asset_bow_ogg));
    sounds.click = sound_load(asset_click_ogg, sizeof(asset_click_ogg));
    sounds.drr = sound_load(asset_drr_ogg, sizeof(asset_drr_ogg));
    sounds.explode = sound_load(asset_explode_ogg, sizeof(asset_explode_ogg));
    sounds.fizz = sound_load(asset_fizz_ogg, sizeof(asset_fizz_ogg));
    sounds.fuse = sound_load(asset_fuse_ogg, sizeof(asset_fuse_ogg));
    sounds.hurt = sound_load(asset_hurt_ogg, sizeof(asset_hurt_ogg));
    sounds.pop = sound_load(asset_pop_ogg, sizeof(asset_pop_ogg));
    sounds.splash = sound_load(asset_splash_ogg, sizeof(asset_splash_ogg));
    sounds.lava = sound_load(asset_lava_ogg, sizeof(asset_lava_ogg));
    sounds.water = sound_load(asset_water_ogg, sizeof(asset_water_ogg));
    sounds.fire = sound_load(asset_fire_ogg, sizeof(asset_fire_ogg));
    sounds.ignite = sound_load(asset_ignite_ogg, sizeof(asset_ignite_ogg));
    sounds.glass[0] = sound_load(asset_glass1_ogg, sizeof(asset_glass1_ogg));
    sounds.glass[1] = sound_load(asset_glass2_ogg, sizeof(asset_glass2_ogg));
    sounds.glass[2] = sound_load(asset_glass3_ogg, sizeof(asset_glass3_ogg));

    sounds.music_volume = 1.0f;
    sounds.sound_volume = 1.0f;

    for(int i = 0; i < 4; i++) {
        alSource3f(sounds.grass[i].source, AL_POSITION, 0, 0, 0);
        alSource3f(sounds.gravel[i].source, AL_POSITION, 0, 0, 0);
        alSource3f(sounds.stone[i].source, AL_POSITION, 0, 0, 0);
        alSource3f(sounds.wood[i].source, AL_POSITION, 0, 0, 0);
    }
    for(int i = 0; i < 3; i++) {
        alSource3f(sounds.sheep[i].source, AL_POSITION, 0, 0, 0);
        alSource3f(sounds.pig[i].source, AL_POSITION, 0, 0, 0);
        alSource3f(sounds.glass[i].source, AL_POSITION, 0, 0, 0);
    }
    alSource3f(sounds.click.source, AL_POSITION, 0, 0, 0);
    alSource3f(sounds.bow.source, AL_POSITION, 0, 0, 0);

    return sounds;
}

void sounds_play_music(sounds_t *sounds, char *music) {

}

void sounds_play_sound(sounds_t *sounds, uint8_t sound, float volume, float pitch) {
    switch(sound) {
        case SOUND_FIRE_FIRE:
            sound_play(&sounds->fire, volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_FIRE_IGNITE:
            sound_play(&sounds->ignite, volume * 0.2 * sounds->sound_volume, pitch);
            return;

        case SOUND_LIQUID_LAVA:
            sound_play(&sounds->lava, volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_LIQUID_WATER:
            sound_play(&sounds->water, volume * 0.2 * sounds->sound_volume, pitch);
            return;

        case SOUND_MOB_PIG:
            sound_play(&sounds->pig[random_next_int_range(&sounds->random, 0, 2)], volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_MOB_PIG_DEATH:
            sound_play(&sounds->pig_death, volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_MOB_SHEEP:
            sound_play(&sounds->sheep[random_next_int_range(&sounds->random, 0, 2)], volume * 0.2 * sounds->sound_volume, pitch);
            return;

        case SOUND_RANDOM_BOW:
            sound_play(&sounds->bow, volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_RANDOM_CLICK:
            sound_play(&sounds->click, volume * 0.25 * sounds->sound_volume, pitch);
            return;
        case SOUND_RANDOM_DRR:
            sound_play(&sounds->drr, volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_RANDOM_EXPLODE:
            sound_play(&sounds->explode, volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_RANDOM_FIZZ:
            sound_play(&sounds->fizz, volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_RANDOM_FUSE:
            sound_play(&sounds->fuse, volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_RANDOM_GLASS:
            sound_play(&sounds->glass[random_next_int_range(&sounds->random, 0, 2)], volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_RANDOM_HURT:
            sound_play(&sounds->hurt, volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_RANDOM_POP:
            sound_play(&sounds->pop, volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_RANDOM_SPLASH:
            sound_play(&sounds->splash, volume * 0.2 * sounds->sound_volume, pitch);
            return;

        case SOUND_STEP_CLOTH:
            sound_play(&sounds->wool[random_next_int_range(&sounds->random, 0, 3)], volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_STEP_GRASS:
            sound_play(&sounds->grass[random_next_int_range(&sounds->random, 0, 3)], volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_STEP_GRAVEL:
            sound_play(&sounds->gravel[random_next_int_range(&sounds->random, 0, 3)], volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_STEP_SAND:
            sound_play(&sounds->sand[random_next_int_range(&sounds->random, 0, 3)], volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_STEP_STONE:
            sound_play(&sounds->stone[random_next_int_range(&sounds->random, 0, 3)], volume * 0.2 * sounds->sound_volume, pitch);
            return;
        case SOUND_STEP_WOOD:
            sound_play(&sounds->wood[random_next_int_range(&sounds->random, 0, 3)], volume * 0.2 * sounds->sound_volume, pitch);
            return;
    }
}

void sounds_destroy(sounds_t *sounds) {
    for(int i = 0; i < 4; i++) {
        sound_destroy(&sounds->grass[i]);
        sound_destroy(&sounds->gravel[i]);
        sound_destroy(&sounds->stone[i]);
        sound_destroy(&sounds->wood[i]);
    }
    for(int i = 0; i < 3; i++) {
        sound_destroy(&sounds->sheep[i]);
        sound_destroy(&sounds->pig[i]);
        sound_destroy(&sounds->glass[i]);
    }
    sound_destroy(&sounds->bow);
    sound_destroy(&sounds->click);
    sound_destroy(&sounds->drr);
    sound_destroy(&sounds->explode);
    sound_destroy(&sounds->fizz);
    sound_destroy(&sounds->fuse);
    sound_destroy(&sounds->hurt);
    sound_destroy(&sounds->pop);
    sound_destroy(&sounds->splash);
    sound_destroy(&sounds->pig_death);
    sound_destroy(&sounds->lava);
    sound_destroy(&sounds->water);
    sound_destroy(&sounds->fire);
    sound_destroy(&sounds->ignite);

    alcMakeContextCurrent(NULL);
    alcDestroyContext(sounds->context);
    alcCloseDevice(sounds->device);
}