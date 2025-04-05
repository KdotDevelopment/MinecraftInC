#pragma once

#include <model/vec3.h>

#include <stdint.h>

typedef struct {
    int type; // 0 = block, 1 = entity
    int x, y, z;
    int face;
    vec3_t location;
    struct entity_s *entity;
    uint8_t null;
} hit_result_t;