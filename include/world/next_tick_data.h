#pragma once

#include <stdint.h>

typedef struct next_tick_data_s {
    uint8_t block_id;
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t delay;
    int ticks;
} next_tick_data_t;