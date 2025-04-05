#pragma once

#include <model/vertex.h>

typedef struct {
    vertex_t vertices[4];
} polygon_t;

polygon_t polygon_create(vertex_t vertices[4], int u1, int v1, int u2, int v2);
polygon_t polygon_create_float(vertex_t vertices[4], float u1, float v1, float u2, float v2);
void polygon_flip(polygon_t *polygon);