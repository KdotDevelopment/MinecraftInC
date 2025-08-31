#pragma once

#include <world/block/block.h>

block_t block_sand_create(uint8_t block_id, int texture_id, float hardness, float resistance);
void block_sand_on_neighbor_placed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t block_id);
void block_sand_on_added(block_t *block, struct world_s *world, int x, int y, int z);