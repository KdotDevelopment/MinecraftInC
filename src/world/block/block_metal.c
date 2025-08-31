#include <world/block/block_metal.h>
#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/material/materials.h>

block_t block_metal_create(uint8_t id, int texture_id, float hardness, float resistance) {
    block_t block = block_create(id, texture_id, &block_sounds.metal, hardness, resistance, &materials.metal);
    block.get_texture_side = block_metal_get_texture_side;

    block_list[block.id] = block;

    return block;
}

int block_metal_get_texture_side(block_t *block, uint8_t face) {
    return face == 1 ? block->texture_id - 16 : (face == 0 ? block->texture_id + 16 : block->texture_id);
}