#include <level/block/block_log.h>
#include <level/block/blocks.h>
#include <level/block/block_sound.h>

block_t block_log_create() {
    block_t block = block_create(BLOCK_LOG, TEXTURE_LOG_SIDE, block_sounds.wood, 1, 2.5, 1);
    block.get_texture_id = block_log_get_texture_id;

    block_list[block.id] = block;

    return block;
}

int block_log_get_texture_id(block_t *block, int face) {
    return face == 1 ? TEXTURE_LOG_TOP : (face == 0 ? TEXTURE_LOG_TOP : TEXTURE_LOG_SIDE);
}