#pragma once

#include <level/block/block.h>

block_t block_glass_create();
uint8_t block_glass_can_render_side(block_t *block, struct level_s *level, int x, int y, int z, int side);