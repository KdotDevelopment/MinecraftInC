#pragma once

#include <world/block/block.h>

block_t block_slab_create(uint8_t id, uint8_t double_slab, float hardness, float persistence);
int block_slab_get_texture_side(block_t *block, uint8_t face);
void block_slab_on_neighbor_changed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t block_id);
void block_slab_on_added(block_t *block, struct world_s *world, int x, int y, int z);
uint8_t block_slab_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side);