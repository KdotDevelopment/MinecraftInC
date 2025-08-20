#pragma once

#include <item/item.h>

item_t item_hoe_create(uint8_t id, uint8_t texture_id, uint8_t hoe_type);
uint8_t item_hoe_on_use(item_stack_t *item_stack, world_t *world, int x, int y, int z, uint8_t side);
