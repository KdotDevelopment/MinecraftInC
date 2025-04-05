#include <util/random.h>
#include <util/sin_table.h>

#include <math.h>
#include <stdlib.h>

random_t random_create(uint64_t seed) {
    random_t random = { 0 };
    random.seed = seed;
    random.state = seed;
    random.last_normal = 0x7ff0000000000000; //infinity
    return random;
}

// Mimics java's RNG behavior
uint64_t random_next_int(random_t *random) {
    random->state ^= (random->state >> 12);
	random->state ^= (random->state << 25);
	random->state ^= (random->state >> 27);
	return random->state * 2685821657736338717ULL;
}

uint64_t random_next_int_range(random_t *random, uint64_t min, uint64_t max) {
	return min + (random_next_int(random) % (max - min + 1));
}

uint64_t random_int_range(uint64_t min, uint64_t max) {
	return rand() % (max + 1) - min;
}

double random_next_uniform(random_t *random) {
    return (double)random_next_int(random) / (double)UINT64_MAX;
}

double random_uniform() {
	return (double)rand() / (double)RAND_MAX;
}

double random_next_normal(random_t *random, double stddev) {
    if (random->last_normal != 0x7ff0000000000000) {
		double r = random->last_normal;
		random->last_normal = 0x7ff0000000000000;
		return r * stddev;
	}

	double r = sqrt(-2.0 * log(1.0 - random_next_uniform(random)));
	double phi = 2.0 * M_PI * (1.0 - random_next_uniform(random));

	random->last_normal = r * tcos(phi);
	return r * tsin(phi) * stddev;
}