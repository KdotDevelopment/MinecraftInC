#include <world/block/block_gears.h>

#include <world/block/blocks.h>

block_t block_gears_create() {
    block_t block = block_create(BLOCK_GEARS, TEXTURE_GEARS, &block_sounds.metal, 0.5, 0.0, &materials.logic);

    block.is_opaque = 0;
    block.render_normal_block = 0;
    block.render_type = BLOCK_RENDER_TYPE_GEARS;
    block.is_collidable = 0;

    block.get_collision_aabb = block_gears_get_collision_aabb;
    block.get_drop_count = block_gears_get_drop_count;

    block_list[block.id] = block;

    return block;
}

AABB_t block_gears_get_collision_aabb(block_t *block, int x, int y, int z) {
    return (AABB_t){ .null = 1 }; 
}

uint8_t block_gears_get_drop_count(block_t *block, random_t *random) {
    return 1;
}