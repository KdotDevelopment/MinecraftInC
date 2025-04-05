#pragma once

#include <level/block/block.h>

block_t block_grass_create();
int block_grass_get_texture_id(block_t *block, int side);
void block_grass_update(block_t *block, struct level_s *level, int x, int y, int z, random_t *random);