#pragma once

#include <item/item.h>
#include <world/block/block.h>

item_t item_sword_create(uint8_t id, uint8_t texture_id, uint8_t sword_type);
float item_sword_get_strength_against_block(item_t *item, block_t *block);
void item_sword_hit_entity(item_t *item, item_stack_t *item_stack);
void item_sword_on_block_destroy(item_t *item, item_stack_t *item_stack);
int item_sword_get_damage_against_entity(item_t *item);