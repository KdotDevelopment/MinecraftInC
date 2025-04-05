#pragma once

#include <level/block/block.h>

block_t block_metal_create(uint8_t id, int texture_id);
int block_metal_get_texture_id(block_t *block, int face);