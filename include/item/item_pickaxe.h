#pragma once

#include <item/item.h>

item_t item_pickaxe_create(uint8_t id, uint8_t texture_id, uint8_t pickaxe_type);
uint8_t item_pickaxe_can_harvest_block(item_t *item, uint8_t block_id);