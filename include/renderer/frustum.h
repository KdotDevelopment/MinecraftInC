#pragma once

#include <physics/AABB.h>

#include <stdint.h>

typedef struct {
    float planes[6][16];
    float projection[16];
    float model_view[16];
    float clip[16];
    double x, y, z;
} frustum_t;

frustum_t frustum_get();
uint8_t frustum_contains_box(frustum_t frustum, float ax, float ay, float az, float bx, float by, float bz);
uint8_t frustum_contains_box_bb(frustum_t frustum, AABB_t box);
void frustum_set_position(frustum_t *frustum, double x, double y, double z);