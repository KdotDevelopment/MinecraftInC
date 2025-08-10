#include <level/block/block_metal.h>
#include <level/block/blocks.h>
#include <level/block/block_sound.h>

block_t block_metal_create(uint8_t id, int texture_id) {
    block_t block = block_create(id, texture_id, block_sounds.metal, 1, id == BLOCK_GOLD ? 3 : 5, 1);
    block.get_texture_id = block_metal_get_texture_id;

    block_list[block.id] = block;

    return block;
}

int block_metal_get_texture_id(block_t *block, int face) {
    return face == 1 ? block->texture_id - 16 : (face == 0 ? block->texture_id + 16 : block->texture_id);
}