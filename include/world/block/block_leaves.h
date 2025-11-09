#pragma once

#include <world/block/block.h>

block_t block_leaves_create();
uint8_t block_leaves_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side);
uint8_t block_leaves_get_drop_count(block_t *block, random_t *random);
int16_t block_leaves_get_item_dropped(block_t *block, uint8_t metadata, random_t *random);