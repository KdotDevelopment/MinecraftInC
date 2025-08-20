#include <world/block/block_stone.h>

#include <world/block/blocks.h>
#include <world/material/materials.h>

block_t block_stone_create(uint8_t id, uint8_t texture_id, float hardness, float resistance) {
    block_t block = block_create(id, texture_id, &block_sounds.stone, hardness, resistance, &materials.rock);

    block.drop_id = BLOCK_COBBLESTONE;

    block_list[id] = block;

    return block;
}