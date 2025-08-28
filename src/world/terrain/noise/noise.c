#include <world/terrain/noise/noise.h>
#include <world/terrain/noise/noise_composite.h>
#include <world/terrain/noise/noise_octave.h>
#include <world/terrain/noise/noise_perlin.h>

noise_t noise_create() {
    noise_t noise;

    noise = (noise_t){ 0 };

    noise.get = noise_get;
    noise.destroy = noise_destroy;

    return noise;
}

double noise_get(noise_t *noise, double x, double y, double z) {
    return 0.0;
}

void noise_destroy(noise_t *noise) {
    return;
}