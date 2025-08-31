#pragma once

#include <world/block/block.h>

block_t block_transparent_create(uint8_t id, int texture_id, block_sound_t *sound, float hardness, float resistance, material_t *material, uint8_t render_sides);
uint8_t block_transparent_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side);