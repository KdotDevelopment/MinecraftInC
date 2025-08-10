#include <level/block/block_bookshelf.h>
#include <level/block/blocks.h>
#include <level/block/block_sound.h>

block_t block_bookshelf_create() {
    block_t block = block_create(BLOCK_BOOKSHELF, TEXTURE_BOOKSHELF, block_sounds.wood, 1, 1.5, 1);
    block.get_texture_id = block_bookshelf_get_texture_id;

    block_list[block.id] = block;

    return block;
}

int block_bookshelf_get_texture_id(block_t *block, int face) {
    return face <= 1 ? TEXTURE_WOOD : block->texture_id;
}