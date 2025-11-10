#include <world/block/block_gravel.h>

#include <item/items.h>
#include <world/block/block_sand.h>
#include <world/block/blocks.h>

block_t block_gravel_create() {
    block_t block = block_sand_create(BLOCK_GRAVEL, TEXTURE_GRAVEL, 0.6, 0);

    block.get_item_dropped = block_gravel_get_item_dropped;

    block_list[block.id] = block;
    
    return block;
}

int16_t block_gravel_get_item_dropped(block_t *block, uint8_t metadata, random_t *random) {
    return random_next_int_range(random, 0, 9) == 0 ? items.flint.id : block->id;
}