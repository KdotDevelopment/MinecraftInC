#pragma once

#include <world/world.h>

#include <stdint.h>

typedef struct block_renderer_s {
    world_t *world;
    int16_t fixed_texture_id;
    uint8_t flip_texture;
} block_renderer_t;

void block_renderer_create(block_renderer_t *block_renderer, world_t *world);