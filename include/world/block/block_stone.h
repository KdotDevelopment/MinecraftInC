#pragma once

#include <world/block/block.h>

block_t block_stone_create(uint8_t id, uint8_t texture_id, float hardness, float resistance);
int16_t block_stone_get_item_dropped(block_t *block, uint8_t metadata, random_t *random);