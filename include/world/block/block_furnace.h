#pragma once

#include <world/block/block.h>
#include <world/block/tile_entity/tile_entity_furnace.h>

#include <stdint.h>

block_t block_furnace_create(uint8_t block_id, uint8_t is_lit);
void block_furnace_on_added(block_t *block, struct world_s *world, int x, int y, int z);
int block_furnace_get_texture(block_t *block, struct world_s *world, int x, int y, int z, uint8_t face);
void block_furnace_visual_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);
int block_furnace_get_texture_side(block_t *block, uint8_t face);
uint8_t block_furnace_on_interacted(block_t *block, struct world_s *world, int x, int y, int z, entity_t *player);
tile_entity_t *block_furnace_get_tile_entity(struct world_s *world);