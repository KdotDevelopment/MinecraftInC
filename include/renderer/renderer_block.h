#pragma once

#include <stdint.h>

struct world_s;

typedef struct renderer_block_s {
    struct world_s *world;
    int16_t fixed_texture_id;
    uint8_t flip_texture;
} renderer_block_t;

void renderer_block_create(renderer_block_t *renderer_block, struct world_s *world);