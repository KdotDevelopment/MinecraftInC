#include <world/block/block_workbench.h>

#include <world/block/blocks.h>
#include <world/world.h>

block_t block_workbench_create() {
    block_t block = block_create(BLOCK_WORKBENCH, TEXTURE_WORKBENCH, &block_sounds.wood, 2.5, 0, &materials.wood);

    block.get_texture_side = block_workbench_get_texture_side;
    block.on_interacted = block_workbench_on_interacted;

    block_list[block.id] = block;

    return block;
}

int block_workbench_get_texture_side(block_t *block, uint8_t face) {
    return face == 1 ? block->texture_id - 16 : (face == 0 ? blocks.wood.texture_id : (face != 2 && face != 4 ? block->texture_id : block->texture_id + 1));
}

uint8_t block_workbench_on_interacted(block_t *block, world_t *world, int x, int y, int z, entity_t *player) {
    player_render_crafting_screen(player);
    return 1;
}