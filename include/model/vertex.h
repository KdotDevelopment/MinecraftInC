#pragma once

#include <model/vec3.h>

typedef struct {
    vec3_t pos;
    float u;
    float v;
} vertex_t;

vertex_t vertex_remap(vertex_t *a, float u, float v);