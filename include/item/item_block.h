#pragma once

#include <item/item.h>
#include <item/item_stack.h>
#include <world/world.h>

item_t item_block_create(int16_t block_id);
uint8_t item_block_on_use(item_t *item, item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side);