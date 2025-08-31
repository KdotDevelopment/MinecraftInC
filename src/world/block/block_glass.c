#include <world/block/block_glass.h>

#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/block/block_transparent.h>
#include <world/world.h>
#include <world/material/materials.h>

block_t block_glass_create() {
    block_t block = block_transparent_create(BLOCK_GLASS, TEXTURE_GLASS, &block_sounds.metal, 0.3, 0, &materials.glass, 0);

    block.get_drop_count = block_glass_get_drop_count;

    block_list[block.id] = block;

    return block;
}

uint8_t block_glass_get_drop_count(block_t *block, random_t *random) {
    return 0;
}