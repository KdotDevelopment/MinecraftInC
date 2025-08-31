#pragma once

#include <world/block/block.h>

block_t block_stationary_create(uint8_t block_id, material_t *material);
void block_stationary_update(block_t *block, world_t *world, int x, int y, int z, random_t *random);
void block_stationary_on_neighbor_changed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t block_id);