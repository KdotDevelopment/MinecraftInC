#pragma once

#include <level/block/block.h>

block_t block_sapling_create();
void block_sapling_update(block_t *block, struct level_s *level, int x, int y, int z, random_t *random);