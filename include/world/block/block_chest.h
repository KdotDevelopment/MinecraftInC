#pragma once

#include <world/block/block.h>
#include <world/block/tile_entity/tile_entity_chest.h>

block_t block_chest_create();
int block_chest_get_texture(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side);
int block_chest_get_texture_side(block_t *block, uint8_t side);
uint8_t block_chest_can_place_at(block_t *block, struct world_s *world, int x, int y, int z);
void block_chest_on_removed(block_t *block, struct world_s *world, int x, int y, int z);
uint8_t block_chest_on_interacted(block_t *block, struct world_s *world, int x, int y, int z, entity_t *player);
tile_entity_t *block_chest_get_tile_entity(struct world_s *world);