#include <level/block/block_tnt.h>
#include <level/block/blocks.h>
#include <level/block/block_sound.h>
#include <particle/particles.h>

block_t block_tnt_create() {
    block_t block = block_create(BLOCK_TNT, TEXTURE_TNT, block_sounds.wool, 1, 0, 1);
    block.get_texture_id = block_tnt_get_texture_id;
    block.destroy = block_tnt_destroy;

    block_list[block.id] = block;

    return block;
}

int block_tnt_get_texture_id(block_t *block, int face) {
    return face == 0 ? block->texture_id + 2 : (face == 1 ? block->texture_id + 1 : block->texture_id);
}

void block_tnt_explode(block_t *block, struct level_s *level, int x, int y, int z) {

}

void block_tnt_destroy(block_t *block, struct level_s *level, int x, int y, int z, particles_t *particles) {

}