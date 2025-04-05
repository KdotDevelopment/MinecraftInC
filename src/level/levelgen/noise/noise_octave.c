#include <level/levelgen/noise/noise_octave.h>
#include <level/levelgen/noise/noise_perlin.h>
#include <level/levelgen/noise/noise.h>
#include <stdlib.h>

noise_t noise_octave_create(random_t *random, int octave_count) {
    noise_t noise = noise_create();
    noise.noise_type = NOISE_OCTAVE;
    noise.octave.count = octave_count;
    noise.octave.noises = calloc(octave_count, sizeof(noise_t));

    for(int i = 0; i < octave_count; i++) {
        noise.octave.noises[i] = noise_perlin_create(random);
    }

    return noise;
}

float noise_octave_get(noise_t *noise, float x, float y) {
    float a = 0.0;
    float b = 1.0;
    for(int i = 0; i < noise->octave.count; i++) {
        a += noise_get(&noise->octave.noises[i], x / b, y / b) * b;
        b *= 2;
    }
    return a;
}

void noise_octave_destroy(noise_t *noise) {
    free(noise->octave.noises);
}