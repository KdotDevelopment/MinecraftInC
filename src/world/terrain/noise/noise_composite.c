#include <world/terrain/noise/noise_composite.h>

noise_t noise_composite_create(noise_t *noise1, noise_t *noise2) {
    noise_t noise = noise_create();

    noise.noise_type = NOISE_COMPOSITE;
    noise.noise1 = noise1;
    noise.noise2 = noise2;

    noise.get = noise_composite_get;

    return noise;
}

double noise_composite_get(noise_t *noise, double x, double y, double z) {
    return noise->noise1->get(noise->noise1, x + noise->noise2->get(noise->noise2, x, y, z), y, z);
}