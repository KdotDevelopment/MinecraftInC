#include <item/item_axe.h>

#include <item/item_tool.h>
#include <item/items.h>
#include <world/block/blocks.h>

item_t item_axe_create(uint8_t id, uint8_t texture_id, uint8_t axe_type) {
    item_t item = item_tool_create(id, PIECE_TOOL_AXE, texture_id);
    
    return item;
}