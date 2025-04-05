#pragma once

#include <stdint.h>

typedef struct {
    int32_t type;
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t delay;
    int ticks;
} next_tick_data_t;