#include <world/block/block_ore.h>

#include <item/items.h>
#include <world/block/blocks.h>
#include <world/material/materials.h>
#include <world/world.h>

block_t block_ore_create(uint8_t id, uint8_t texture_id, float hardness, float resistance) {
    block_t block = block_create(id, texture_id, &block_sounds.stone, hardness, resistance, &materials.rock);

    block.get_item_dropped = block_ore_get_item_dropped;
    block.get_drop_count = block_ore_get_drop_count;

    block_list[id] = block;

    return block;
}

int16_t block_ore_get_item_dropped(block_t *block, uint8_t metadata, random_t *random) {
    if(block->id == BLOCK_COAL_ORE) return items.coal.id;
    if(block->id == BLOCK_DIAMOND_ORE) return items.diamond.id;
    return block->id;
}

uint8_t block_ore_get_drop_count(block_t *block, random_t *random) {
    return 1;
}