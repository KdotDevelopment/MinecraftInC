#pragma once

#include <world/terrain/noise/noise.h>

noise_t noise_perlin_create(random_t *random);
double noise_perlin_get(noise_t *noise, double x, double y, double z);
void noise_perlin_populate_array(noise_t *noise, double *output, int start_x, int start_y, int start_z, int size_x, int size_y, int size_z, double scale_x, double scale_y, double scale_z, double amplitude);