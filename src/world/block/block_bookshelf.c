#include <world/block/block_bookshelf.h>
#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/material/materials.h>

block_t block_bookshelf_create() {
    block_t block = block_create(BLOCK_BOOKSHELF, TEXTURE_BOOKSHELF, &block_sounds.wood, 1.5, 0, &materials.wood);
    block.get_texture_id = block_bookshelf_get_texture_id;

    block_list[block.id] = block;

    return block;
}

int block_bookshelf_get_texture_id(block_t *block, int face) {
    return face <= 1 ? TEXTURE_WOOD : block->texture_id;
}