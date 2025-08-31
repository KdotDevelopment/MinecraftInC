#pragma once

#include <world/block/block.h>

block_t block_torch_create();
AABB_t block_torch_get_collision_aabb(block_t *block, int x, int y, int z);
uint8_t block_torch_can_place_at(block_t *block, struct world_s *world, int x, int y, int z);
void block_torch_on_placed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side);
void block_torch_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);
void block_torch_on_added(block_t *block, struct world_s *world, int x, int y, int z);
void block_torch_on_neighbor_changed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t block_id);
hit_result_t block_torch_clip(block_t *block, struct world_s *world, int x, int y, int z, vec3_t v0, vec3_t v1);
void block_torch_visual_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);