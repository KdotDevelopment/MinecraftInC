#include <world/block/block_crops.h>

#include <entity/entity_item.h>
#include <item/items.h>
#include <item/item_stack.h>
#include <world/block/blocks.h>
#include <world/block/block_flower.h>
#include <world/world.h>

#include <stdio.h>
#include <stdlib.h>

block_t block_crops_create() {
    block_t block = block_flower_create(BLOCK_CROPS, TEXTURE_CROPS);

    block.should_tick = 1;
    block.render_type = BLOCK_RENDER_TYPE_CROPS;
    block_set_bounds(&block, 0.0, 0.0, 0.0, 1.0, 0.25, 1.0);

    block.can_grow_on = block_crops_can_grow_on;
    block.update = block_crops_update;
    block.get_texture_side_metadata = block_crops_get_texture_side_metadata;
    block.on_destroyed = block_crops_on_destroyed;
    block.get_item_dropped = block_crops_get_item_dropped;
    block.get_drop_count = block_crops_get_drop_count;

    block_list[block.id] = block;

    return block;
}

uint8_t block_crops_can_grow_on(block_t *block, uint8_t block_id) {
    return block_id == BLOCK_FARMLAND;
}

void block_crops_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    block_flower_update(block, world, x, y, z, random);
    if(world_get_block_light_value(world, x, y, z) >= 9) {
        uint8_t growth_stage = world_get_block_metadata(world, x, y, z);
        if(growth_stage < 7) {
            float growth_chance = 1.0;

            uint8_t block_north = world_get_block(world, x, y, z - 1);
            uint8_t block_south = world_get_block(world, x, y, z + 1);
            uint8_t block_east = world_get_block(world, x + 1, y, z);
            uint8_t block_west = world_get_block(world, x - 1, y, z);
            uint8_t block_north_west = world_get_block(world, x - 1, y, z - 1);
            uint8_t block_north_east = world_get_block(world, x + 1, y, z - 1);
            uint8_t block_south_east = world_get_block(world, x + 1, y, z + 1);
            uint8_t block_south_west = world_get_block(world, x - 1, y, z + 1);

            uint8_t has_adjacent_crops_x = block_west == block->id || block_east == block->id;
            uint8_t has_adjacent_crops_z = block_north == block->id || block_south == block->id;
            uint8_t has_diagonal_crops = block_north_west == block->id || block_north_east == block->id 
                                      || block_south_east == block->id || block_south_west == block->id;

            for(int check_x = x - 1; check_x <= x + 1; check_x++) {
                for(int check_z = z - 1; check_z <= z + 1; check_z++) {
                    uint8_t farmland = world_get_block(world, check_x, y - 1, check_z);
                    float farmland_modifier = 0.0;

                    if(farmland == BLOCK_FARMLAND) {
                        farmland_modifier = 1.0;

                        if(world_get_block_metadata(world, check_x, y - 1, check_z) > 0) {
                            farmland_modifier = 3.0;
                        }
                    }

                    if(check_x != x || check_z != z) {
                        farmland_modifier /= 4.0;
                    }

                    growth_chance += farmland_modifier;
                }
            }

            if(has_diagonal_crops || (has_adjacent_crops_x && has_adjacent_crops_z)) {
                growth_chance /= 2.0;
            }

            if(random_next_int_range(random, 0, (100.0 / growth_chance) - 1) == 0) {
                growth_stage++;
                world_set_block_metadata(world, x, y, z, growth_stage);
            }
        }
    }
}

int block_crops_get_texture_side_metadata(block_t *block, uint8_t side, uint8_t metadata) {
    if(metadata == 0) metadata = 7;
    return block->texture_id + metadata;
}

void block_crops_on_destroyed(block_t *block, world_t *world, int x, int y, int z, uint8_t metadata) {
    block_on_destroyed(block, world, x, y, z, metadata);

    for(int i = 0; i < 3; i++) {
        if(random_next_int_range(&world->random, 0, 14) <= metadata) {
            float offset_x = random_next_uniform(&world->random) * 0.7 + 0.15;
            float offset_y = random_next_uniform(&world->random) * 0.7 + 0.15;
            float offset_z = random_next_uniform(&world->random) * 0.7 + 0.15;

            entity_t *item = malloc(sizeof(entity_t));
            item_stack_t item_stack = item_stack_create(items.seeds.item_id, 1, 0);
            entity_item_create(item, world, x + offset_x, y + offset_y, z + offset_z, item_stack);
            item->delay_before_pickup = 10;
            world_spawn_entity(world, item);
        }
    }
}

int16_t block_crops_get_item_dropped(block_t *block, uint8_t metadata, random_t *random) {
    // Must be some debugging left over from Notch
    printf("Get resource: %d\n", metadata);
    return metadata == 7 ? items.seeds.item_id : -1;
}

uint8_t block_crops_get_drop_count(block_t *block, random_t *random) {
    return 1;
}