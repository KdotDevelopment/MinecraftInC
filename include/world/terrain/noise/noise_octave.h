#include <world/terrain/noise/noise.h>

noise_t noise_octave_create(random_t *random, int octave_count);
double noise_octave_get(noise_t *noise, double x, double y, double z);
double *noise_octave_generate_octaves(noise_t *noise, double *array, int start_x, int start_y, int start_z, int size_x, int size_y, int size_z, double scale_x, double scale_y, double scale_z);
void noise_octave_destroy(noise_t *noise);