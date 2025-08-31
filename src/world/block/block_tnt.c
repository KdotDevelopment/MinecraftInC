#include <world/block/block_tnt.h>

#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <particle/particles.h>
#include <world/material/materials.h>

block_t block_tnt_create() {
    block_t block = block_create(BLOCK_TNT, TEXTURE_TNT, &block_sounds.wool, 0, 0, &materials.tnt);
    block.get_texture_side = block_tnt_get_texture_side;
    block.on_destroyed = block_tnt_on_destroyed;

    block_list[block.id] = block;

    return block;
}

int block_tnt_get_texture_side(block_t *block, uint8_t face) {
    return face == 0 ? block->texture_id + 2 : (face == 1 ? block->texture_id + 1 : block->texture_id);
}

void block_tnt_explode(block_t *block, struct world_s *world, int x, int y, int z) {

}

void block_tnt_on_destroyed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t metadata) {

}