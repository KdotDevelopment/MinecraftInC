#pragma once

#include <world/block/block.h>

block_t block_sponge_create();
void block_sponge_on_added(block_t *block, struct world_s *world, int x, int y, int z);
void block_sponge_on_removed(block_t *block, struct world_s *world, int x, int y, int z);