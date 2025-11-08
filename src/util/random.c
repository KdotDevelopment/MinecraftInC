#include <util/random.h>
#include <util/math_helper.h>

#include <math.h>
#include <stdlib.h>

#define JAVA_RAND_MULTIPLIER 0x5DEECE66DULL
#define JAVA_RAND_ADDEND     0xBULL
#define JAVA_RAND_MASK       ((1ULL << 48) - 1)

random_t random_create(uint64_t seed) {
    random_t random = { 0 };

    random_set_seed(&random, seed);
    random.state = random.seed;
    random.last_normal = 0x7ff0000000000000; //infinity
    
    return random;
}

int32_t random_next_bits(random_t *random, int bits) {
    random->seed = (random->seed * JAVA_RAND_MULTIPLIER + JAVA_RAND_ADDEND) & JAVA_RAND_MASK;
    return (int32_t)(random->seed >> (48 - bits));
}

// Mimics java's RNG behavior
uint64_t random_next_int(random_t *random) {
    return random_next_bits(random, 32);
}

uint64_t random_next_int_range(random_t *random, uint64_t min, uint64_t max) {
    return min + (random_next_int(random) % (max - min + 1));
}

uint64_t random_int_range(uint64_t min, uint64_t max) {
    return rand() % (max + 1) - min;
}

double random_next_uniform(random_t *random) {
    int64_t a = random_next_bits(random, 26);
    int64_t b = random_next_bits(random, 27);
    return (((int64_t)a << 27) + b) / (double)(1ULL << 53);
}

double random_uniform() {
    return (double)rand() / (double)RAND_MAX;
}

double random_next_normal(random_t *random, double stddev) {
    if(random->last_normal != 0x7ff0000000000000) {
        double r = random->last_normal;
        random->last_normal = 0x7ff0000000000000;
        return r * stddev;
    }

    double r = sqrt(-2.0 * log(1.0 - random_next_uniform(random)));
    double phi = 2.0 * M_PI * (1.0 - random_next_uniform(random));

    random->last_normal = r * tcos(phi);
    return r * tsin(phi) * stddev;
}

int64_t random_next_long(random_t *random) {
    int64_t high = (int64_t)random_next_bits(random, 32);
    int64_t low  = (int64_t)random_next_bits(random, 32);
    return (high << 32) + (low & 0xFFFFFFFFL);
}

void random_set_seed(random_t *random, long seed) {
    random->seed = (seed ^ 0x5DEECE66DL) & ((1L << 48) - 1);
}