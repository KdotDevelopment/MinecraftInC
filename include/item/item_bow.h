#pragma once

#include <item/item.h>

item_t item_bow_create(uint8_t id, uint8_t texture_id);
item_stack_t item_bow_on_right_click(item_stack_t *item_stack, world_t *world, player_t *player);