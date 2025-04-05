#pragma once

#include <stdint.h>

typedef struct {
    float planes[6][16];
    float projection[16];
    float model_view[16];
    float clip[16];
} frustum_t;

frustum_t frustum_get();
uint8_t frustum_contains_box(frustum_t frustum, float ax, float ay, float az, float bx, float by, float bz);