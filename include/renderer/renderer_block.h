#pragma once

#include <world/world.h>

#include <stdint.h>

typedef struct renderer_block_s {
    world_t *world;
    int16_t fixed_texture_id;
    uint8_t flip_texture;
} renderer_block_t;

void renderer_block_create(renderer_block_t *renderer_block, world_t *world);