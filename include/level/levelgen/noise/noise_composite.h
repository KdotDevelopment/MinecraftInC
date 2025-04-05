#pragma once

#include <level/levelgen/noise/noise.h>

noise_t noise_composite_create(noise_t *noise1, noise_t *noise2);
float noise_composite_get(noise_t *noise, float x, float y);