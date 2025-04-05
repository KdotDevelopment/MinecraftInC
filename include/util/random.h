#pragma once

#include <stdint.h>

#define M_PI 3.14159265358979

typedef struct {
    uint64_t seed;
    uint64_t state;
    double last_normal;
} random_t;

random_t random_create(uint64_t seed);
uint64_t random_next_int(random_t *random);
uint64_t random_int_range(uint64_t min, uint64_t max);
double random_next_uniform(random_t *random);
double random_uniform();
uint64_t random_next_int_range(random_t *random, uint64_t min, uint64_t max);
double random_next_normal(random_t *random, double stddev);