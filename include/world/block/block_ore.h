#pragma once

#include <world/block/block.h>

block_t block_ore_create(uint8_t id, uint8_t texture_id, float hardness, float resistance);
int16_t block_ore_get_item_dropped(block_t *block, uint8_t metadata, random_t *random);
uint8_t block_ore_get_drop_count(block_t *block, random_t *random);