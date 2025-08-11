#pragma once

#include <stdint.h>

typedef struct material_s {
    uint8_t is_liquid;
    uint8_t is_solid;
    uint8_t can_block_grass;
} material_t;

material_t material_create();