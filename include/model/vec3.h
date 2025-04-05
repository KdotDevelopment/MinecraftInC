#pragma once

#include <math.h>
#include <stdint.h>

#define vec3_null (vec3_t){ INFINITY, INFINITY, INFINITY }

typedef struct {
    float x;
    float y;
    float z;
} vec3_t;

vec3_t vec3_vector_to(vec3_t a, vec3_t b);
vec3_t vec3_normalize(vec3_t a);
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_subtract(vec3_t a, vec3_t b);
float vec3_distance_to(vec3_t a, vec3_t b);
float vec3_distance_to_sqr(vec3_t a, vec3_t b);
float vec3_length(vec3_t a);
vec3_t vec3_clip_x(vec3_t a, vec3_t b, float lerp);
vec3_t vec3_clip_y(vec3_t a, vec3_t b, float lerp);
vec3_t vec3_clip_z(vec3_t a, vec3_t b, float lerp);
static inline uint8_t vec3_is_null(vec3_t v) {
	return v.x == INFINITY && v.y == INFINITY && v.z == INFINITY;
}