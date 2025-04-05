#pragma once

#include <stdint.h>

typedef struct {
    float ticks_per_second;
    float last_hr;
    int elapsed_ticks;
    float delta;
    float speed;
    float elapsed_delta;
    uint64_t last_system_clock;
    uint64_t last_hr_clock;
    float adjustment;
} game_timer_t;

game_timer_t timer_create(float ticks_per_second);