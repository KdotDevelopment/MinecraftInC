#pragma once

#include <world/block/block.h>

block_t block_glass_create();
uint8_t block_glass_get_drop_count(block_t *block, random_t *random);