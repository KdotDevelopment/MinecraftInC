#pragma once

#include <item/item.h>
#include <item/item_stack.h>

item_t item_soup_create(int16_t id, uint8_t texture_id, int16_t heal_amount);
item_stack_t item_soup_on_right_click(item_stack_t *item_stack, world_t *world, player_t *player);