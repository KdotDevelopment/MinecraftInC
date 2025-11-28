#include <world/block/block_fire.h>

#include <sound/sounds.h>
#include <world/block/blocks.h>
#include <world/material/materials.h>
#include <world/world.h>

void private_set_block_burn_rate(uint8_t block_id, uint8_t spread_chance, uint8_t catch_chance) {
    block_list[block_id].fire_spread_chance = spread_chance;
    block_list[block_id].fire_catch_chance = catch_chance;
    ((block_t *)&blocks)[block_id].fire_spread_chance = spread_chance;
    ((block_t *)&blocks)[block_id].fire_catch_chance = catch_chance;
}

block_t block_fire_create() {
    block_t block = block_create(BLOCK_FIRE, TEXTURE_FIRE, &block_sounds.wood, 0.0, 0.0, &materials.fire);

    block.light_value = 15;
    block.should_tick = 1;
    block.is_opaque = 0;
    block.light_opacity = 0;
    block.render_normal_block = 0;
    block.render_type = BLOCK_RENDER_TYPE_FIRE;
    block.tick_rate = 20;
    block.is_collidable = 0;

    private_set_block_burn_rate(blocks.wood.id, 5, 20);
    private_set_block_burn_rate(blocks.log.id, 5, 5);
    private_set_block_burn_rate(blocks.leaves.id, 30, 60);
    private_set_block_burn_rate(blocks.bookshelf.id, 30, 20);
    private_set_block_burn_rate(blocks.tnt.id, 15, 100);

    for(int i = 0; i < 16; i++) {
        private_set_block_burn_rate(blocks.red_wool.id + i, 30, 60);
    }

    block.get_collision_aabb = block_fire_get_collision_aabb;
    block.get_drop_count = block_fire_get_drop_count;
    block.update = block_fire_update;
    block.can_place_at = block_fire_can_place_at;
    block.on_neighbor_changed = block_fire_on_neighbor_changed;
    block.on_added = block_fire_on_added;
    block.visual_update = block_fire_visual_update;

    block_list[block.id] = block;

    return block;
}

AABB_t block_fire_get_collision_aabb(block_t *block, int x, int y, int z) {
    return (AABB_t){ .null = 1 };
}

uint8_t block_fire_get_drop_count(block_t *block, random_t *random) {
    return 0;
}

uint8_t block_fire_can_catch_fire(world_t *world, int x, int y, int z) {
    return block_list[world_get_block(world, x, y, z)].fire_catch_chance > 0;
}

uint8_t private_can_neighbor_catch_fire(world_t *world, int x, int y, int z) {
    return block_fire_can_catch_fire(world, x + 1, y, z) ? 1 :
        block_fire_can_catch_fire(world, x - 1, y, z) ? 1 :
        block_fire_can_catch_fire(world, x, y - 1, z) ? 1 :
        block_fire_can_catch_fire(world, x, y + 1, z) ? 1 :
        block_fire_can_catch_fire(world, x, y, z - 1) ? 1 :
        block_fire_can_catch_fire(world, x, y, z + 1);
}

void private_try_spread_fire(world_t *world, int x, int y, int z, int chance, random_t *random) {
    int block_chance = block_list[world_get_block(world, x, y, z)].fire_catch_chance;
    if(random_next_int_range(random, chance) < block_chance) {
        uint8_t is_tnt = world_get_block(world, x, y, z) == blocks.tnt.id;
        if(random_next_int_range(random, 2) == 0) {
            world_set_block_with_update(world, x, y, z, blocks.fire.id);
        }else {
            world_set_block_with_update(world, x, y, z, blocks.air.id);
        }

        if(is_tnt) {
            blocks.tnt.on_destroyed(&blocks.tnt, world, x, y, z, 0);
        }
    }
}

int private_get_fire_spread_chance(world_t *world, int x, int y, int z, int chance) {
    int block_chance = block_list[world_get_block(world, x, y, z)].fire_spread_chance;
    return block_chance > chance ? block_chance : chance;
}

void block_fire_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    uint8_t metadata = world_get_block_metadata(world, x, y, z);
    if(metadata < 15) {
        world_set_block_metadata(world, x, y, z, metadata + 1);
        world_schedule_block_update(world, x, y, z, block->id);
    }

    if(!private_can_neighbor_catch_fire(world, x, y, z)) {
        if(!world_is_solid(world, x, y - 1, z) || metadata > 3) {
            world_set_block_with_update(world, x, y, z, blocks.air.id);
        }
    }else if(!block_fire_can_catch_fire(world, x, y - 1, z) && metadata == 15 && random_next_int_range(random, 4) == 0) {
        world_set_block_with_update(world, x, y, z, blocks.air.id);
    }else {
        if(metadata % 5 == 0 && metadata > 5) {
            private_try_spread_fire(world, x + 1, y, z, 300, random);
            private_try_spread_fire(world, x - 1, y, z, 300, random);
            private_try_spread_fire(world, x, y - 1, z, 100, random);
            private_try_spread_fire(world, x, y + 1, z, 200, random);
            private_try_spread_fire(world, x, y, z - 1, 300, random);
            private_try_spread_fire(world, x, y, z + 1, 300, random);
            
            for(int rx = x - 1; rx <= x + 1; rx++) {
                for(int rz = z - 1; rz <= z + 1; rz++) {
                    for(int ry = y - 1; ry <= y + 4; ry++) {
                        if(rx != x || ry != y || rz != z) {
                            int chance = 100;
                            if(ry > y + 1) {
                                chance = 100 + (ry - (y + 1)) * 100;
                            }

                            int spread_chance = 0;
                            if(world_get_block(world, rx, ry, rz) != 0) {
                                spread_chance = 0;
                            }else {
                                spread_chance = private_get_fire_spread_chance(world, rx + 1, ry, rz, spread_chance);
                                spread_chance = private_get_fire_spread_chance(world, rx - 1, ry, rz, spread_chance);
                                spread_chance = private_get_fire_spread_chance(world, rx, ry - 1, rz, spread_chance);
                                spread_chance = private_get_fire_spread_chance(world, rx, ry + 1, rz, spread_chance);
                                spread_chance = private_get_fire_spread_chance(world, rx, ry, rz - 1, spread_chance);
                                spread_chance = private_get_fire_spread_chance(world, rx, ry, rz + 1, spread_chance);
                            }

                            if(spread_chance > 0 && random_next_int_range(random, chance) <= spread_chance) {
                                world_set_block_with_update(world, rx, ry, rz, block->id);
                            }
                        }
                    }
                }
            }
        }
    }
}

uint8_t block_fire_can_place_at(block_t *block, world_t *world, int x, int y, int z) {
    return world_is_solid(world, x, y - 1, z) || private_can_neighbor_catch_fire(world, x, y, z);
}

void block_fire_on_neighbor_changed(block_t *block, world_t *world, int x, int y, int z, uint8_t block_id) {
    if(!world_is_solid(world, x, y - 1, z) && !private_can_neighbor_catch_fire(world, x, y, z)) {
        world_set_block_with_update(world, x, y, z, blocks.air.id);
    }
}

void block_fire_on_added(block_t *block, world_t *world, int x, int y, int z) {
    if(!world_is_solid(world, x, y - 1, z) && !private_can_neighbor_catch_fire(world, x, y, z)) {
        world_set_block_with_update(world, x, y, z, blocks.air.id);
    }else {
        world_schedule_block_update(world, x, y, z, block->id);
    }
}

uint8_t block_fire_can_neighbor_spread_fire(uint8_t block_id) {
    return block_list[block_id].fire_spread_chance > 0;
}

uint8_t private_fire_check(world_t *world, int x, int y, int z) {
    uint8_t block_id = world_get_block(world, x, y, z);
    if(block_id == blocks.fire.id) {
        return 1;
    }else if(block_id == blocks.air.id) {
        world_set_block_with_update(world, x, y, z, blocks.fire.id);
    }
    return 0;
}

void block_fire_spread(world_t *world, int x, int y, int z) {
    uint8_t is_spread = 0;
    is_spread = private_fire_check(world, x, y + 1, z);
    if(!is_spread) is_spread = private_fire_check(world, x - 1, y, z);
    if(!is_spread) is_spread = private_fire_check(world, x + 1, y, z);
    if(!is_spread) is_spread = private_fire_check(world, x, y, z - 1);
    if(!is_spread) is_spread = private_fire_check(world, x, y, z + 1);
    if(!is_spread) is_spread = private_fire_check(world, x, y - 1, z);
    if(!is_spread) world_set_block_with_update(world, x, y, z, blocks.fire.id);
}

void block_fire_visual_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    if(random_next_int_range(random, 24) == 0) {
        world_play_sound(world, x + 0.5, y + 0.5, z + 0.5, SOUND_FIRE_FIRE, 1.0 + random_next_double(random), random_next_double(random) * 0.7 + 0.3);
    }

    float particle_x = 0;
    float particle_y = 0;
    float particle_z = 0;

    if(!world_is_solid(world, x, y - 1, z) && !block_fire_can_catch_fire(world, x, y - 1, z)) {
        if(block_fire_can_catch_fire(world, x - 1, y, z)) {
            for(int i = 0; i < 2; i++) {
                particle_x = x + random_next_double(random) * 0.1;
                particle_y = y + random_next_double(random);
                particle_z = z + random_next_double(random);
                world_spawn_particle(world, PARTICLE_LARGE_SMOKE, particle_x, particle_y, particle_z, 0, 0, 0);
            }
        }

        if(block_fire_can_catch_fire(world, x + 1, y, z)) {
            for(int i = 0; i < 2; i++) {
                particle_x = (x + 1) + random_next_double(random) * 0.1;
                particle_y = y + random_next_double(random);
                particle_z = z + random_next_double(random);
                world_spawn_particle(world, PARTICLE_LARGE_SMOKE, particle_x, particle_y, particle_z, 0, 0, 0);
            }
        }

        if(block_fire_can_catch_fire(world, x, y, z - 1)) {
            for(int i = 0; i < 2; i++) {
                particle_x = x + random_next_double(random);
                particle_y = y + random_next_double(random);
                particle_z = z + random_next_double(random) * 0.1;
                world_spawn_particle(world, PARTICLE_LARGE_SMOKE, particle_x, particle_y, particle_z, 0, 0, 0);
            }
        }

        if(block_fire_can_catch_fire(world, x, y, z + 1)) {
            for(int i = 0; i < 2; i++) {
                particle_x = x + random_next_double(random) * 0.1;
                particle_y = y + random_next_double(random);
                particle_z = (z + 1) + random_next_double(random) * 0.1;
                world_spawn_particle(world, PARTICLE_LARGE_SMOKE, particle_x, particle_y, particle_z, 0, 0, 0);
            }
        }

        if(block_fire_can_catch_fire(world, x, y + 1, z)) {
            for(int i = 0; i < 2; i++) {
                particle_x = x + random_next_double(random);
                particle_y = (y + 1) + random_next_double(random) * 0.1;
                particle_z = z + random_next_double(random);
                world_spawn_particle(world, PARTICLE_LARGE_SMOKE, particle_x, particle_y, particle_z, 0, 0, 0);
            }
        }
    } else {
        for(int i = 0; i < 3; i++) {
            particle_x = x + random_next_double(random);
            particle_y = y + random_next_double(random) * 0.5 + 0.5;
            particle_z = z + random_next_double(random);
            world_spawn_particle(world, PARTICLE_LARGE_SMOKE, particle_x, particle_y, particle_z, 0, 0, 0);
        }
    }
}