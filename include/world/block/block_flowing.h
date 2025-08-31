#pragma once

#include <world/block/block.h>

block_t block_flowing_create(uint8_t block_id, material_t *material);
void block_flowing_on_added(block_t *block, struct world_s *world, int x, int y, int z);
void block_flowing_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);
uint8_t block_flowing_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side);
AABB_t block_flowing_get_collision_aabb(block_t *block, int x, int y, int z);