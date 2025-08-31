#pragma once

#include <world/block/block.h>

block_t block_crops_create();
uint8_t block_crops_can_grow_on(block_t *block, uint8_t block_id);
void block_crops_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);
int block_crops_get_texture_side_metadata(block_t *block, uint8_t side, uint8_t metadata);
void block_crops_on_destroyed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t metadata);
int16_t block_crops_get_item_dropped(block_t *block, uint8_t metadata, random_t *random);
uint8_t block_crops_get_drop_count(block_t *block, random_t *random);
