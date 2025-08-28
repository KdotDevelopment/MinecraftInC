#include <world/terrain/noise/noise_octave.h>

#include <world/terrain/noise/noise_perlin.h>
#include <world/terrain/noise/noise.h>

#include <stdlib.h>
#include <string.h>

noise_t noise_octave_create(random_t *random, int octave_count) {
    noise_t noise = noise_create();
    noise.noise_type = NOISE_OCTAVE;
    noise.count = octave_count;
    noise.noises = calloc(octave_count, sizeof(noise_t));

    for(int i = 0; i < octave_count; i++) {
        noise.noises[i] = noise_perlin_create(random);
    }

    noise.get = noise_octave_get;
    noise.destroy = noise_octave_destroy;

    return noise;
}

double noise_octave_get(noise_t *noise, double x, double y, double z) {
    double a = 0.0;
    double b = 1.0;

    for(int i = 0; i < noise->count; i++) {
        a += noise->noises[i].get(&noise->noises[i], x * b, y * b, z * b) / b;
        b /= 2;
    }

    return a;
}

double *noise_octave_generate_octaves(noise_t *noise, double *array, int start_x, int start_y, int start_z, int size_x, int size_y, int size_z, double scale_x, double scale_y, double scale_z) {
    if(array == NULL) {
        array = malloc(size_x * size_y * size_z * sizeof(double));
    }else {
        memset(array, 0, size_x * size_y * size_z * sizeof(double));
    }

    double amplitude = 1.0;

    for(int i = 0; i < noise->count; i++) {
        noise_perlin_populate_array(&noise->noises[i], array, start_x, start_y, start_z, size_x, size_y, size_z, scale_x * amplitude, scale_y * amplitude, scale_z * amplitude, amplitude);
        amplitude /= 2.0;
    }

    return array;
}

void noise_octave_destroy(noise_t *noise) {
    free(noise->noises);
}