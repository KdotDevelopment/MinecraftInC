#pragma once

#include <world/block/block.h>

block_t block_tnt_create();
int block_tnt_get_texture_side(block_t *block, uint8_t face);
void block_tnt_explode(block_t *block, struct world_s *world, int x, int y, int z);
void block_tnt_on_destroyed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t metadata);