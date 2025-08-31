#pragma once

#include <world/block/block.h>

block_t block_spawner_create(uint8_t block_id, uint8_t spawn_id);
void block_spawner_on_added(block_t *block, struct world_s *world, int x, int y, int z);
void block_spawner_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);