#include <renderer/renderer_block.h>

#include <world/world.h>

void renderer_block_create(renderer_block_t *renderer_block, world_t *world) {
    *renderer_block = (renderer_block_t){ 0 };
    renderer_block->world = world;
    renderer_block->fixed_texture_id = -1;
    renderer_block->flip_texture = 0;
}