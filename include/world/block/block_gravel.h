#pragma once

#include <world/block/block.h>

block_t block_gravel_create();
int16_t block_gravel_get_item_dropped(block_t *block, uint8_t metadata, random_t *random);