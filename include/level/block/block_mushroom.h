#pragma once

#include <level/block/block.h>

block_t block_mushroom_create(uint8_t id, int texture_id);
void block_mushroom_update(block_t *block, struct level_s *level, int x, int y, int z, random_t *random);