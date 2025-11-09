#include <item/item_pickaxe.h>

#include <item/item.h>
#include <item/items.h>
#include <item/item_tool.h>
#include <world/material/materials.h>

item_t item_pickaxe_create(uint8_t id, uint8_t texture_id, uint8_t pickaxe_type) {
    item_t item = item_tool_create(id, texture_id, PIECE_TOOL_PICKAXE, pickaxe_type);

    item.can_harvest_block = item_pickaxe_can_harvest_block;

    item_list[id + 256] = item;

    return item;
}

uint8_t item_pickaxe_can_harvest_block(item_t *item, uint8_t block_id) {
    switch(block_id) {
        case BLOCK_OBSIDIAN:
            return item->tool_type >= TYPE_TOOL_DIAMOND;
        case BLOCK_DIAMOND:
        case BLOCK_DIAMOND_ORE:
        case BLOCK_GOLD:
        case BLOCK_GOLD_ORE:
            return item->tool_type >= TYPE_TOOL_IRON;
        case BLOCK_IRON:
        case BLOCK_IRON_ORE:
            return item->tool_type >= TYPE_TOOL_STONE;
        default: {
            block_t *block = &block_list[block_id];
            return block->material->id == materials.rock.id || block->material->id == materials.metal.id;
        }
    }

    return 0;
}