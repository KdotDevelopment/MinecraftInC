#pragma once

#include <stdint.h>

#include <util/random.h>

#define NOISE_NONE 0
#define NOISE_PERLIN 1
#define NOISE_OCTAVE 2
#define NOISE_COMPOSITE 3

typedef struct noise_s {
    uint8_t noise_type;
    union {
        struct {
            int hash[512];
        } perlin;
        struct {
            int count;
            struct noise_s *noises;
        } octave;
        struct {
            struct noise_s *noise1;
            struct noise_s *noise2;
        } combined;
    };
} noise_t;

noise_t noise_create();
void noise_destroy(noise_t *noise);
float noise_get(noise_t *noise, float x, float y);