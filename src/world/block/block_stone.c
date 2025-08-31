#include <world/block/block_stone.h>

#include <world/block/blocks.h>
#include <world/material/materials.h>

block_t block_stone_create(uint8_t id, uint8_t texture_id, float hardness, float resistance) {
    block_t block = block_create(id, texture_id, &block_sounds.stone, hardness, resistance, &materials.rock);

    block.get_item_dropped = block_stone_get_item_dropped;

    block_list[block.id] = block;

    return block;
}

int16_t block_stone_get_item_dropped(block_t *block, uint8_t metadata, random_t *random) {
    return blocks.cobblestone.id;
}