#pragma once

#include <level/block/block.h>

block_t block_sand_create(uint8_t block_id, int texture_id);
void block_sand_on_neighbor_placed(block_t *block, struct level_s *level, int x, int y, int z, uint8_t block_id);
void block_sand_on_placed(block_t *block, struct level_s *level, int x, int y, int z);