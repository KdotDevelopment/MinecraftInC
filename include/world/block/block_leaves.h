#pragma once

#include <world/block/block.h>

block_t block_leaves_create();
uint8_t block_leaves_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side);