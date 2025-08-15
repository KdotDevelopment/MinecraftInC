#include <world/block/block_sponge.h>
#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/world.h>

block_t block_sponge_create() {
    block_t block = block_create(BLOCK_SPONGE, TEXTURE_SPONGE, block_sounds.wool, 0.9, 0.6, 1);
    block.on_added = block_sponge_on_added;
    block.on_removed = block_sponge_on_removed;

    block_list[block.id] = block;

    return block;
}

void block_sponge_on_added(block_t *block, struct world_s *world, int x, int y, int z) {
    world_t *real_world = (world_t *)world;
    for(int i = x - 2; i <= x + 2; i++) {
        for(int j = y - 2; j <= y + 2; j++) {
            for(int k = z - 2; k <= z + 2; k++) {
                if(world_is_water(real_world, i, j, k)) world_set_block_no_neighbor_change(real_world, i, j, k, blocks.air.id);
            }
        }
    }
}

void block_sponge_on_removed(block_t *block, struct world_s *world, int x, int y, int z) {
    world_t *real_world = (world_t *)world;
    for(int i = x - 2; i <= x + 2; i++) {
        for(int j = y - 2; j <= y + 2; j++) {
            for(int k = z - 2; k <= z + 2; k++) {
                world_update_neighbors_at(real_world, i, j, k, world_get_block(real_world, i, j, k));
            }
        }
    }
}