#pragma once

#include <world/block/block.h>

block_t block_metal_create(uint8_t id, int texture_id, float hardness, float resistance);
int block_metal_get_texture_id(block_t *block, int face);