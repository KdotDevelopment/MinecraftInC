#pragma once

#include <item/item.h>
#include <world/block/blocks.h>
#include <world/block/block.h>

item_t item_tool_create(uint8_t id, uint8_t texture_id, uint8_t tool_piece, uint8_t tool_type);
float item_tool_get_strength(item_t *item, block_t *block);
void item_tool_hit_entity(item_t *item, item_stack_t *item_stack);
void item_tool_on_block_destroy(item_t *item, item_stack_t *item_stack);