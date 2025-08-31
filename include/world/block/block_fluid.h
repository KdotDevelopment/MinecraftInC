#pragma once

#include <world/block/block.h>

block_t block_fluid_create(uint8_t block_id, material_t *material);
int block_fluid_get_texture_side(block_t *block, uint8_t face);
void block_fluid_on_added(block_t *block, struct world_s *world, int x, int y, int z);
uint8_t block_fluid_can_flow(block_t *block, struct world_s *world, int x, int y, int z);
void block_fluid_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);
float block_fluid_get_brightness(block_t *block, struct world_s *world, int x, int y, int z);
uint8_t block_fluid_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side);
AABB_t block_fluid_get_collision_aabb(block_t *block, int x, int y, int z);
void block_fluid_on_neighbor_changed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t block_id);
int block_fluid_get_tick_rate(block_t *block);
uint8_t block_fluid_get_drop_count(block_t *block, random_t *random);
void block_fluid_visual_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);