#pragma once

#include <level/levelgen/noise/noise.h>

noise_t noise_perlin_create(random_t *random);
float noise_perlin_get(noise_t *noise, float x, float y);