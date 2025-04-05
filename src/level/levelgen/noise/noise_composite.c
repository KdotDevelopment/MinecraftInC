#include <level/levelgen/noise/noise_composite.h>

noise_t noise_composite_create(noise_t *noise1, noise_t *noise2) {
    noise_t noise = noise_create();
    noise.noise_type = NOISE_COMPOSITE;
    noise.combined.noise1 = noise1;
    noise.combined.noise2 = noise2;
    return noise;
}

float noise_composite_get(noise_t *noise, float x, float y) {
    return noise_get(noise->combined.noise1, x + noise_get(noise->combined.noise2, x, y), y);
}