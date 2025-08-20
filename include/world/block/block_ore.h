#pragma once

#include <world/block/block.h>

block_t block_ore_create(uint8_t id, uint8_t texture_id, float hardness, float resistance);

uint8_t block_ore_get_drop_count(block_t *block, world_t *world);