#pragma once

#include <world/block/block.h>
#include <world/material/material.h>

block_t block_container_create(uint8_t id, int texture_id, block_sound_t *sound, float hardness, float resistance, material_t *material);
void block_container_on_added(block_t *block, struct world_s *world, int x, int y, int z);
void block_container_on_removed(block_t *block, struct world_s *world, int x, int y, int z);
void block_container_set_keep_inventory(uint8_t keep_inventory);