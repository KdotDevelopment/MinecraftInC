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
        // Perlin
        struct {
            int hash[512];
            double x_coord;
            double y_coord;
            double z_coord;
        };

        // Octave
        struct {
            int count;
            struct noise_s *noises;
        };

        // Composite
        struct {
            struct noise_s *noise1;
            struct noise_s *noise2;
        };
    };

    double (*get)(struct noise_s *noise, double x, double y, double z);
    void (*destroy)(struct noise_s *noise);
} noise_t;

noise_t noise_create();
void noise_destroy(noise_t *noise);
double noise_get(noise_t *noise, double x, double y, double z);