#pragma once

#include <world/block/block.h>

block_t block_gears_create();
AABB_t block_gears_get_collision_aabb(block_t *block, int x, int y, int z);
uint8_t block_gears_get_drop_count(block_t *block, random_t *random);