#pragma once

#include <world/terrain/noise/noise.h>

noise_t noise_composite_create(noise_t *noise1, noise_t *noise2);
double noise_composite_get(noise_t *noise, double x, double y, double z);