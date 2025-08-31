#pragma once

#include <world/block/block.h>
#include <physics/AABB.h>

block_t block_flower_create(uint8_t id, int texture_id);
uint8_t block_flower_can_grow_on(block_t *block, uint8_t block_id);
uint8_t block_flower_can_stay(block_t *block, struct world_s *world, int x, int y, int z);
void block_flower_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);
AABB_t block_flower_get_collision_aabb(block_t *block, int x, int y, int z);
void block_flower_render_preview(block_t *block);
uint8_t block_flower_render(block_t *block, struct world_s *world, int x, int y, int z);
void block_flower_render_full_brightness(block_t *block);