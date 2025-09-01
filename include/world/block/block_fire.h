#pragma once

#include <world/block/block.h>

block_t block_fire_create();
AABB_t block_fire_get_collision_aabb(block_t *block, int x, int y, int z);
uint8_t block_fire_get_drop_count(block_t *block, random_t *random);
uint8_t block_fire_can_catch_fire(struct world_s *world, int x, int y, int z);
void block_fire_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);
uint8_t block_fire_can_place_at(block_t *block, struct world_s *world, int x, int y, int z);
void block_fire_on_neighbor_changed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t block_id);
void block_fire_on_added(block_t *block, struct world_s *world, int x, int y, int z);
uint8_t block_fire_can_neighbor_spread_fire(uint8_t block_id);
void block_fire_spread(struct world_s *world, int x, int y, int z);
void block_fire_visual_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);