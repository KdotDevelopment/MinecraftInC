#include <renderer/renderer_item.h>

#include <string.h>

renderer_item_t renderer_item_create(world_t *world) {
    renderer_item_t renderer_item = { 0 };
    memset(&renderer_item, 0, sizeof(renderer_item_t));

    renderer_item.renderer_block = renderer_block_create(world);

    return renderer_item;
}

void renderer_item_render(renderer_item_t *renderer_item, float partial_tick) {
    
}