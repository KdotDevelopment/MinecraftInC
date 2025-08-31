#include <world/block/block_slab.h>
#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/world.h>
#include <world/material/materials.h>

block_t block_slab_create(uint8_t id, uint8_t double_slab, float hardness, float persistence) {
    block_t block = block_create(id, TEXTURE_SLAB, &block_sounds.stone, hardness, persistence, &materials.rock);
    block.is_solid = !!double_slab;
    block.is_cube = !!double_slab;
    if(!double_slab) block_set_bounds(&block, 0, 0, 0, 1, 0.5, 1);

    block.get_texture_side = block_slab_get_texture_side;
    block.on_neighbor_changed = block_slab_on_neighbor_changed;
    block.on_added = block_slab_on_added;
    block.can_render_side = block_slab_can_render_side;

    block_list[block.id] = block;

    return block;
}

int block_slab_get_texture_side(block_t *block, uint8_t face) {
    return face <= 1 ? TEXTURE_DOUBLE_SLAB : TEXTURE_SLAB;
}

void block_slab_on_neighbor_changed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t block_id) {

}

void block_slab_on_added(block_t *block, struct world_s *world, int x, int y, int z) {
    world_t *real_world = (world_t *)world;
    if(world_get_block(real_world, x, y - 1, z) == blocks.slab.id) {
        world_set_block_with_update(real_world, x, y, z, blocks.air.id);
        world_set_block_with_update(real_world, x, y - 1, z, blocks.double_slab.id);
    }
}

uint8_t block_slab_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side) {
    world_t *real_world = (world_t *)world;
    return side == 1 ? 1 : world_is_solid(real_world, x, y, z) ? 0 : (side == 0 ? 1 : world_get_block(real_world, x, y, z) != block->id);
}