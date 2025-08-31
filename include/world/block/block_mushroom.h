#pragma once

#include <world/block/block.h>

block_t block_mushroom_create(uint8_t id, int texture_id);
uint8_t block_mushroom_can_grow_on(block_t *block, uint8_t block_id);
uint8_t block_mushroom_can_stay(block_t *block, struct world_s *world, int x, int y, int z);