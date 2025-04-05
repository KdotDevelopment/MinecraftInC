#pragma once

#include <level/block/block.h>

block_t block_tnt_create();
int block_tnt_get_texture_id(block_t *block, int face);
void block_tnt_explode(block_t *block, struct level_s *level, int x, int y, int z);
void block_tnt_destroy(block_t *block, struct level_s *level, int x, int y, int z, particles_t *particles);