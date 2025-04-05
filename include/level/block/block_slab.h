#pragma once

#include <level/block/block.h>

block_t block_slab_create(uint8_t id, uint8_t double_slab);
int block_slab_get_texture_id(block_t *block, int face);
void block_slab_on_neighbor_changed(block_t *block, struct level_s *level, int x, int y, int z, uint8_t block_id);
void block_slab_on_added(block_t *block, struct level_s *level, int x, int y, int z);
uint8_t block_slab_can_render_side(block_t *block, struct level_s *level, int x, int y, int z, int side);