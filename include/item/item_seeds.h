#pragma once

#include <item/item.h>

// block_type = (just) wheat
item_t item_seeds_create(uint8_t id, uint8_t texture_id, uint8_t block_type);
uint8_t item_seeds_on_use(item_t *item, item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side);