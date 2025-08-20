#include <item/item_tool.h>

#include <item/items.h>

static uint8_t axe_effective_blocks[] = { BLOCK_PLANKS, BLOCK_BOOKSHELF, BLOCK_LOG, BLOCK_CHEST };
static uint8_t pickaxe_effective_blocks[] = { BLOCK_COBBLESTONE, BLOCK_DOUBLE_SLAB, BLOCK_SLAB, BLOCK_STONE,
    BLOCK_MOSSY_COBBLESTONE, BLOCK_IRON_ORE, BLOCK_IRON, BLOCK_COAL_ORE, BLOCK_GOLD, BLOCK_GOLD_ORE, 
    BLOCK_DIAMOND_ORE, BLOCK_DIAMOND };
static uint8_t shovel_effective_blocks[] = { BLOCK_GRASS, BLOCK_DIRT, BLOCK_SAND, BLOCK_GRAVEL };

item_t item_tool_create(uint8_t id, uint8_t texture_id, uint8_t tool_piece, uint8_t tool_type) {
    item_t item = item_create(id, texture_id);
    
    item.tool_type = tool_type;
    item.tool_piece = tool_piece;
    item.proper_tool_efficiency = (tool_piece + 1) / 2;
    item.durability = 32 << tool_type;
    if(tool_type == 3) {
        item.durability <<= 1;
    }
    item.entity_damage = tool_piece + tool_type;

    item_list[id + 256] = item;

    return item;
}

float item_tool_get_strength(item_t *item, block_t *block) {
    if(item->tool_type == PIECE_TOOL_SHOVEL) {
        for(int i = 0; i < sizeof(shovel_effective_blocks); i++) {
            if(shovel_effective_blocks[i] == block->id) {
                return item->proper_tool_efficiency;
            }
        }
    }
    if(item->tool_type == PIECE_TOOL_PICKAXE) {
        for(int i = 0; i < sizeof(pickaxe_effective_blocks); i++) {
            if(pickaxe_effective_blocks[i] == block->id) {
                return item->proper_tool_efficiency;
            }
        }
    }
    if(item->tool_type == PIECE_TOOL_AXE) {
        for(int i = 0; i < sizeof(axe_effective_blocks); i++) {
            if(axe_effective_blocks[i] == block->id) {
                return item->proper_tool_efficiency;
            }
        }
    }
    return 1.0;
}