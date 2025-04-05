#include <level/levelgen/noise/noise.h>

noise_t noise_octave_create(random_t *random, int octave_count);
float noise_octave_get(noise_t *noise, float x, float y);
void noise_octave_destroy(noise_t *noise);