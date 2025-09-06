#include <world/block/block_fluid.h>

#include <sound/sounds.h>
#include <world/block/blocks.h>
#include <world/block/block_fire.h>
#include <world/block/block_sound.h>
#include <world/material/materials.h>
#include <world/world.h>

block_t block_fluid_create(uint8_t block_id, material_t *material) {
    block_t block = block_create(block_id, TEXTURE_WATER, &block_sounds.none, 0, 2.0, material);

    block_set_bounds(&block, 0.01, -0.09, 0.01, 1.01, 0.90999997, 1.01);
    block.texture_id = block.material == &materials.lava ? TEXTURE_LAVA : TEXTURE_WATER;
    block.liquid_moving_id = block_id;
    block.liquid_still_id = block_id + 1;
    block.should_tick = 1;
    block.render_normal_block = 0;
    block.is_collidable = 0;
    block.is_opaque = 0;
    block.render_pass = block.material == &materials.water ? 1 : 0;
    block.light_value = block.material == &materials.lava ? 15 : 0;
    block.light_opacity = block.material == &materials.lava ? 255 : 3;

    block.get_texture_side = block_fluid_get_texture_side;
    block.on_added = block_fluid_on_added;
    block.update = block_fluid_update;
    block.get_brightness = block_fluid_get_brightness;
    block.can_render_side = block_fluid_can_render_side;
    block.get_collision_aabb = block_fluid_get_collision_aabb;
    block.on_neighbor_changed = block_fluid_on_neighbor_changed;
    block.get_tick_rate = block_fluid_get_tick_rate;
    block.get_drop_count = block_fluid_get_drop_count;
    block.visual_update = block_fluid_visual_update;

    block_list[block.id] = block;

    return block;
}

int block_fluid_get_texture_side(block_t *block, uint8_t face) {
    return block->material == &materials.lava ? block->texture_id : (face == 1 ? block->texture_id : (face == 0 ? block->texture_id : block->texture_id + 32));
}

void block_fluid_on_added(block_t *block, world_t *world, int x, int y, int z) {
    world_schedule_block_update(world, x, y, z, block->liquid_moving_id);
}

uint8_t block_fluid_can_flow(block_t *block, world_t *world, int x, int y, int z) {
    if(block->material == &materials.water) {
        for(int i = x - 2; i <= x + 2; i++) {
            for(int j = y - 2; j <= y + 2; j++) {
                for(int k = z - 2; k <= z + 2; k++) {
                    if(world_get_block(world, i, j, k) == blocks.sponge.id) return 0;
                }
            }
        }
    }

    return 1;
}

uint8_t can_flow(block_t *block, world_t *world, int x, int y, int z) {
    if(!(!world_get_block_material(world, x, y, z)->is_liquid && !world_get_block_material(world, x, y, z)->is_solid)) return 0;
    if(block->material == &materials.water) {
        for(int i = x - 2; i <= x + 2; i++) {
            for(int j = y - 2; j <= y + 2; j++) {
                for(int k = z - 2; k <= z + 2; k++) {
                    if(world_get_block(world, i, j, k) == blocks.sponge.id) return 0;
                }
            }
        }
    }

    return 1;
}

uint8_t flow(block_t *block, world_t *world, int x, int y, int z) {
    if(!can_flow(block, world, x, y, z)) return 0;
    if(world_set_block_with_update(world, x, y, z, block->liquid_moving_id)) {
        world_schedule_block_update(world, x, y, z, block->liquid_moving_id);
    }
    return 0;
}

uint8_t lava_spread_fire(world_t *world, int x, int y, int z) {
    if(block_fire_can_neighbor_spread_fire(world_get_block(world, x, y, z))) {
        block_fire_spread(world, x, y, z);
        return 1;
    }else {
        return 0;
    }
}

void block_fluid_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    return;
    uint8_t set = 0;
    uint8_t can_set = 0;
    do {
        y--;
        if(!can_flow(block, world, x, y, z)) {
            break;
        }
        can_set = world_set_block_with_update(world, x, y, z, block->liquid_moving_id);
        if(can_set) set = 1;
    }while(can_set && block->material != &materials.lava);

    y++;
    if(block->material == &materials.water || !set) {
        set = set | flow(block, world, x - 1, y, z) | flow(block, world, x + 1, y, z) | flow(block, world, x, y, z - 1) | flow(block, world, x, y, z + 1);
    }
    if(block->material == &materials.lava) {
        set = set | lava_spread_fire(world, x - 1, y, z) | lava_spread_fire(world, x + 1, y, z) | lava_spread_fire(world, x, y, z - 1) | lava_spread_fire(world, x, y, z + 1);
    }
    if(!set) {
        world_set_block_no_update(world, x, y, z, block->liquid_still_id);
    }else {
        world_schedule_block_update(world, x, y, z, block->liquid_moving_id);
    }

    //return set;
}

float block_fluid_get_brightness(block_t *block, world_t *world, int x, int y, int z) {
    return block->material == &materials.lava ? 100.0 : block_get_brightness(block, world, x, y, z);
}

uint8_t block_fluid_can_render_side(block_t *block, world_t *world, int x, int y, int z, uint8_t side) {
    uint8_t block_id = world_get_block(world, x, y, z);
    if(block_id != block->liquid_moving_id && block_id != block->liquid_still_id) {
        if(side == 1) {
            uint8_t air_neighbor = world_get_block(world, x - 1, y, z) == 0 ||
                world_get_block(world, x + 1, y, z) == 0 ||
                world_get_block(world, x, y, z - 1) == 0 ||
                world_get_block(world, x, y, z + 1) == 0;
            if(air_neighbor) return 1;
        }
        return block_can_render_side(block, world, x, y, z, side);
    }else {
        return 0;
    }
}

AABB_t block_fluid_get_collision_aabb(block_t *block, int x, int y, int z) {
    return (AABB_t){ .null = 1 };
}

void block_fluid_on_neighbor_changed(block_t *block, world_t *world, int x, int y, int z, uint8_t block_id) {
    if(block_id != 0) {
        material_t *material = world_get_block_material(world, x, y, z);
        if((block->material == &materials.water && material == &materials.lava) || (material == &materials.water && block->material == &materials.lava)) {
            world_set_block_with_update(world, x, y, z, blocks.stone.id);
        }
    }

    world_schedule_block_update(world, x, y, z, block->id);
}

int block_fluid_get_tick_rate(block_t *block) {
    return block->material == &materials.lava ? 25 : 5;
}

uint8_t block_fluid_get_drop_count(block_t *block, random_t *random) {
    return 0;
}

uint8_t private_liquid_air_check(world_t *world, int x, int y, int z) {
    material_t *material_1 = world_get_block_material(world, x, y, z);
    material_t *material_2 = world_get_block_material(world, x, y - 1, z);
    return !material_1->is_solid && !material_1->is_liquid ? material_2->is_solid || material_2->is_liquid : 0;
}

void block_fluid_visual_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    // This appears to have been intentionally disabled with the -1
    if(random_next_int_range(random, 0, 127) == -1 && world_get_block_material(world, x, y + 1, z)->is_solid) {
        if(block->material == &materials.lava) {
            world_play_sound(world, x + 0.5, y + 0.5, z + 0.5, SOUND_LIQUID_LAVA, random_next_uniform(random) * 0.25 + 12.0 / 16.0, random_next_uniform(random) * 0.5 + 0.3);
        }

        if(block->material == &materials.water) {
            world_play_sound(world, x + 0.5, y + 0.5, z + 0.5, SOUND_LIQUID_WATER, random_next_uniform(random) * 0.25 + 12.0 / 16.0, random_next_uniform(random) + 0.5);
        }
    }

    if(block->material == &materials.lava && world_get_block_material(world, x, y + 1, z) == &materials.air && !world_is_solid(world, x, y + 1, z) && random_next_int_range(random, 0, 99) == 0) {
        float px = x + random_next_uniform(random);
        float py = y + block->y1;
        float pz = z + random_next_uniform(random);
        world_spawn_particle(world, PARTICLE_LAVA, px, py, pz, 0, 0, 0);
    }

    if(block->material == &materials.water) {
        if(private_liquid_air_check(world, x + 1, y, z)) {
            for(int i = 0; i < 4; i++) {
                float px = x + 1 + (2.0 / 16.0);
                float py = y + block->y1;
                float pz = z + random_next_uniform(random);
                world_spawn_particle(world, PARTICLE_WATER_DROP, px, py, pz, 0, 0, 0);
            }
        }

        if(private_liquid_air_check(world, x - 1, y, z)) {
            for(int i = 0; i < 4; i++) {
                float px = x - (2.0 / 16.0);
                float py = y + block->y1;
                float pz = z + random_next_uniform(random);
                world_spawn_particle(world, PARTICLE_WATER_DROP, px, py, pz, 0, 0, 0);
            }
        }

        if(private_liquid_air_check(world, x, y, z + 1)) {
            for(int i = 0; i < 4; i++) {
                float px = x + random_next_uniform(random);
                float py = y + block->y1;
                float pz = z + 1 + (2.0 / 16.0);
                world_spawn_particle(world, PARTICLE_WATER_DROP, px, py, pz, 0, 0, 0);
            }
        }

        if(private_liquid_air_check(world, x, y, z - 1)) {
            for(int i = 0; i < 4; i++) {
                float px = x + random_next_uniform(random);
                float py = y + block->y1;
                float pz = z - (2.0 / 16.0);
                world_spawn_particle(world, PARTICLE_WATER_DROP, px, py, pz, 0, 0, 0);
            }
        }
    }
}