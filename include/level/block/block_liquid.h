#pragma once

#include <level/block/block.h>

block_t block_liquid_create(int block_type, int liquid_type);
void block_liquid_on_placed(block_t *block, struct level_s *level, int x, int y, int z);
void block_liquid_update(block_t *block, struct level_s *level, int x, int y, int z, random_t *random);
float block_liquid_get_brightness(block_t *block, struct level_s *level, int x, int y, int z);
uint8_t block_liquid_can_render_side(block_t *block, struct level_s *level, int x, int y, int z, int side);
void block_liquid_render_inside(block_t *block, int x, int y, int z, int side);
void block_liquid_on_neighbor_changed(block_t *block, struct level_s *level, int x, int y, int z, uint8_t block_id);
AABB_t block_liquid_get_collision_aabb(block_t *block, int x, int y, int z);