#pragma once

#include <level/block/block.h>

block_t block_still_liquid_create(int block_type, int liquid_type);
void block_still_liquid_update(block_t *block, struct level_s *level, int x, int y, int z, random_t *random);
void block_still_liquid_on_neighbor_changed(block_t *block, struct level_s *level, int x, int y, int z, uint8_t block_id);