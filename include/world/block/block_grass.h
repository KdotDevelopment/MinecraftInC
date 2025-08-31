#pragma once

#include <world/block/block.h>

block_t block_grass_create();
int block_grass_get_texture_side(block_t *block, uint8_t side);
void block_grass_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);
int16_t block_grass_get_item_dropped(block_t *block, uint8_t metadata, random_t *random);