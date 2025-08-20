#pragma once

#include <item/item.h>

item_t item_food_create(uint8_t id, uint8_t texture_id, uint8_t heal_amount);
item_stack_t item_food_on_right_click(item_stack_t *item_stack, world_t *world, player_t *player);