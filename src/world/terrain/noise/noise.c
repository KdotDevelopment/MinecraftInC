#include <level/levelgen/noise/noise.h>
#include <level/levelgen/noise/noise_composite.h>
#include <level/levelgen/noise/noise_octave.h>
#include <level/levelgen/noise/noise_perlin.h>

noise_t noise_create() {
    noise_t noise;
    noise = (noise_t){ 0 };
    return noise;
}

float noise_get(noise_t *noise, float x, float y) {
    if(noise->noise_type == NOISE_PERLIN) {
        return noise_perlin_get(noise, x, y);
    } else if(noise->noise_type == NOISE_OCTAVE) {
        return noise_octave_get(noise, x, y);
    } else if(noise->noise_type == NOISE_COMPOSITE) {
        return noise_composite_get(noise, x, y);
    }
    return 0.0;
}

void noise_destroy(noise_t *noise) {
    if(noise->noise_type == NOISE_OCTAVE) {
        noise_octave_destroy(noise);
    }
}