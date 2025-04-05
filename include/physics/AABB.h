#pragma once

#include <player/hit_result.h>

#include <stdint.h>

typedef struct {
    float x0;
    float y0;
    float z0;
    float x1;
    float y1;
    float z1;
    uint8_t null;
} AABB_t;

AABB_t AABB_expand(AABB_t a, float x, float y, float z);
AABB_t AABB_grow(AABB_t a, float x, float y, float z);
AABB_t AABB_move(AABB_t a, float x, float y, float z);
float AABB_get_size(AABB_t a);
float AABB_clip_x_collide(AABB_t a, AABB_t b, float x);
float AABB_clip_y_collide(AABB_t a, AABB_t b, float y);
float AABB_clip_z_collide(AABB_t a, AABB_t b, float z);
uint8_t AABB_intersects(AABB_t a, AABB_t b);
uint8_t AABB_intersects_inner(AABB_t a, AABB_t b);
hit_result_t AABB_clip(AABB_t c, vec3_t v0, vec3_t v1);