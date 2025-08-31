#pragma once

#include <world/block/block.h>

block_t block_metal_create(uint8_t id, int texture_id, float hardness, float resistance);
int block_metal_get_texture_side(block_t *block, uint8_t face);