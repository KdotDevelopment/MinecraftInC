#include <level/block/block_slab.h>
#include <level/block/blocks.h>
#include <level/block/block_sound.h>
#include <level/level.h>

block_t block_slab_create(uint8_t id, uint8_t double_slab) {
    block_t block = block_create(id, TEXTURE_SLAB, block_sounds.stone, 1, 2, 1);
    block.is_solid = !!double_slab;
    block.is_cube = !!double_slab;
    if(!double_slab) block_set_bounds(&block, 0, 0, 0, 1, 0.5, 1);

    block.get_texture_id = block_slab_get_texture_id;
    block.on_neighbor_changed = block_slab_on_neighbor_changed;
    block.on_added = block_slab_on_added;
    block.can_render_side = block_slab_can_render_side;

    block_list[block.id] = block;

    return block;
}

int block_slab_get_texture_id(block_t *block, int face) {
    return face <= 1 ? TEXTURE_DOUBLE_SLAB : TEXTURE_SLAB;
}

void block_slab_on_neighbor_changed(block_t *block, struct level_s *level, int x, int y, int z, uint8_t block_id) {

}

void block_slab_on_added(block_t *block, struct level_s *level, int x, int y, int z) {
    level_t *real_level = (level_t *)level;
    if(level_get_block(real_level, x, y - 1, z) == blocks.slab.id) {
        level_set_block(real_level, x, y, z, blocks.air.id);
        level_set_block(real_level, x, y - 1, z, blocks.double_slab.id);
    }
}

uint8_t block_slab_can_render_side(block_t *block, struct level_s *level, int x, int y, int z, int side) {
    level_t *real_level = (level_t *)level;
    return side == 1 ? 1 : level_is_solid_block(real_level, x, y, z) ? 0 : (side == 0 ? 1 : level_get_block(real_level, x, y, z) != block->id);
}