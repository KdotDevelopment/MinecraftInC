#pragma once

#include <world/block/block.h>

block_t block_farmland_create();
AABB_t block_farmland_get_collision_aabb(block_t *block, int x, int y, int z);
int block_farmland_get_texture_side_metadata(block_t *block, uint8_t side, uint8_t metadata);
void block_farmland_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);
void block_farmland_on_walked_upon(block_t *block, struct world_s *world, int x, int y, int z);
void block_farmland_on_neighbor_changed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t block_id);
int16_t block_farmland_get_item_dropped(block_t *block, uint8_t metadata, random_t *random);