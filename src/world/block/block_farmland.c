#include <world/block/block_farmland.h>

#include <world/block/blocks.h>
#include <world/material/materials.h>
#include <world/world.h>

block_t block_farmland_create() {
    block_t block = block_create(BLOCK_FARMLAND, TEXTURE_FARMLAND, &block_sounds.gravel, 0.6, 0, &materials.ground);

    block_set_bounds(&block, 0.0, 0.0, 0.0, 1.0, 15.0 / 16.0, 1.0);
    block.should_tick = 1;
    block.light_opacity = 255;
    block.is_opaque = 0;
    block.render_normal_block = 0;

    block.get_collision_aabb = block_farmland_get_collision_aabb;
    block.get_texture_side_metadata = block_farmland_get_texture_side_metadata;
    block.update = block_farmland_update;
    block.on_walked_upon = block_farmland_on_walked_upon;
    block.on_neighbor_changed = block_farmland_on_neighbor_changed;
    block.get_item_dropped = block_farmland_get_item_dropped;

    block_list[block.id] = block;

    return block;
}

AABB_t block_farmland_get_collision_aabb(block_t *block, int x, int y, int z) {
    return (AABB_t){ x, y, z, x + 1.0, y + 1.0, z + 1.0 };
}

int block_farmland_get_texture_side_metadata(block_t *block, uint8_t side, uint8_t metadata) {
    return side == 1 && metadata > 0 ? block->texture_id - 1 : (side == 1 ? block->texture_id : 2);
}

void block_farmland_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    if(random_next_int_range(random, 5) == 0) {
        uint8_t has_water = 0;

        for(int check_x = x - 4; check_x <= x + 4 && has_water == 0; check_x++) {
            for(int check_y = y; check_y <= y + 1 && has_water == 0; check_y++) {
                for(int check_z = z - 4; check_z <= z + 4 && has_water == 0; check_z++) {
                    if(world_get_block_material(world, check_x, check_y, check_z) == &materials.water) {
                        has_water = 1;
                        break;
                    }
                }
            }
        }

        if(has_water) {
            world_set_block_metadata(world, x, y, z, 7);
            return;
        }

        int hydration_level = world_get_block_metadata(world, x, y, z);
        if(hydration_level > 0) {
            world_set_block_metadata(world, x, y, z, hydration_level - 1);
            return;
        }

        uint8_t has_crops = 0;
        for(int check_x = x - 4; check_x <= x + 4 && has_crops == 0; check_x++) {
            for(int check_z = z - 4; check_z <= z + 4 && has_crops == 0; check_z++) {
                if(world_get_block(world, check_x, y + 1, check_z) == blocks.crops.id) {
                    has_crops = 1;
                    break;
                }
            }
        }

        if(!has_crops) {
            world_set_block_with_update(world, x, y, z, blocks.dirt.id);
        }
    }
}

void block_farmland_on_walked_upon(block_t *block, world_t *world, int x, int y, int z) {
    if(random_next_int_range(&world->random, 4) == 0) {
        world_set_block_with_update(world, x, y, z, blocks.dirt.id);
    }
}

void block_farmland_on_neighbor_changed(block_t *block, world_t *world, int x, int y, int z, uint8_t block_id) {
    block_on_neighbor_changed(block, world, x, y, z, block_id);
    material_t *material = world_get_block_material(world, x, y + 1, z);
    if(material->is_solid) {
        world_set_block_with_update(world, x, y, z, blocks.dirt.id);
    }
}

int16_t block_farmland_get_item_dropped(block_t *block, uint8_t metadata, random_t *random) {
    return blocks.dirt.get_item_dropped(&blocks.dirt, 0, random);
}