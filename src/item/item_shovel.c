#include <item/item_shovel.h>

#include <item/items.h>
#include <item/item_tool.h>
#include <world/material/materials.h>

item_t item_shovel_create(uint8_t id, uint8_t texture_id, uint8_t shovel_type) {
    item_t item = item_tool_create(id, texture_id, PIECE_TOOL_SHOVEL, shovel_type);

    item_list[id + 256] = item;
    
    return item;
}