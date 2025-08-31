#pragma once

#include <world/block/block.h>

block_t block_workbench_create();
int block_workbench_get_texture_side(block_t *block, uint8_t face);
uint8_t block_workbench_on_interacted(block_t *block, struct world_s *world, int x, int y, int z, entity_t *player);