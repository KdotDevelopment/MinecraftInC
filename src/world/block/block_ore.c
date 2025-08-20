#include <world/block/block_ore.h>

#include <item/items.h>
#include <world/block/blocks.h>
#include <world/material/materials.h>

block_t block_ore_create(uint8_t id, uint8_t texture_id, float hardness, float resistance) {
    block_t block = block_create(id, texture_id, &block_sounds.stone, hardness, resistance, &materials.rock);

    if(id == BLOCK_COAL_ORE) block.drop_id = items.coal.item_id;
    else if(id == BLOCK_DIAMOND_ORE) block.drop_id = items.diamond.item_id;
    else block.drop_id = id;

    block.get_drop_count = block_ore_get_drop_count;

    block_list[id] = block;

    return block;
}

uint8_t block_ore_get_drop_count(block_t *block, world_t *world) {
    return 1;
}