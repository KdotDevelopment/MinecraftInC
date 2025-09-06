#include <world/block/block_grass.h>

#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/world.h>
#include <world/material/materials.h>

block_t block_grass_create() {
    block_t block = block_create(BLOCK_GRASS, TEXTURE_GRASS, &block_sounds.grass, 0.6, 0, &materials.ground);
    block.has_physics = 1;
    block.should_tick = 1;

    block.get_texture_side = block_grass_get_texture_side;
    block.update = block_grass_update;
    block.get_item_dropped = block_grass_get_item_dropped;

    block_list[block.id] = block;

    return block;
}

int block_grass_get_texture_side(block_t *block, uint8_t side) {
    return side == 1 ? TEXTURE_GRASS : (side == 0 ? TEXTURE_DIRT : TEXTURE_GRASS_SIDE);
}

void block_grass_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random) {
    world_t *real_world = (world_t *)world;
    return;
    if(world_get_block_light_value(real_world, x, y + 1, z) < 4 && world_get_block_material(real_world, x, y + 1, z)->can_block_grass) {
        if(random_next_int_range(random, 0, 3) == 0) {
            world_set_block_with_update(real_world, x, y, z, blocks.dirt.id);
        }
    }else {
        if(world_get_block_light_value(real_world, x, y + 1, z) >= 9) {
            int xx = x + random_next_int_range(random, 0, 2) - 1;
            int yy = y + random_next_int_range(random, 0, 4) - 3;
            int zz = z + random_next_int_range(random, 0, 2) - 1;
            if(world_get_block(real_world, xx, yy, zz) == blocks.dirt.id 
               && world_get_block_light_value(real_world, xx, yy + 1, zz) >= 4 
               && !world_get_block_material(real_world, xx, yy + 1, zz)->can_block_grass) {
                world_set_block_with_update(real_world, xx, yy, zz, blocks.grass.id);
            }
        }
    }
}

int16_t block_grass_get_item_dropped(block_t *block, uint8_t metadata, random_t *random) {
    return blocks.dirt.get_item_dropped(&blocks.dirt, 0, random);
}