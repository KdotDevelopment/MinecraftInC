#include <renderer/block_renderer.h>

void block_renderer_create(block_renderer_t *block_renderer, world_t *world) {
    *block_renderer = (block_renderer_t){ 0 };
    block_renderer->world = world;
    block_renderer->fixed_texture_id = -1;
    block_renderer->flip_texture = 0;
}