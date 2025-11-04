#include <world/block/block_torch.h>

#include <world/block/blocks.h>
#include <world/material/materials.h>
#include <world/world.h>

block_t block_torch_create() {
    block_t block = block_create(BLOCK_TORCH, TEXTURE_TORCH, &block_sounds.wood, 0, 0, &materials.wood);

    block.is_opaque = 0;
    block.light_opacity = 0;
    block.is_solid = 0;
    block.render_normal_block = 0;
    block.render_type = BLOCK_RENDER_TYPE_TORCH;
    block.light_value = 14;
    block.should_tick = 1;

    block.get_collision_aabb = block_torch_get_collision_aabb;
    block.can_place_at = block_torch_can_place_at;
    block.on_placed = block_torch_on_placed;
    block.update = block_torch_update;
    block.on_added = block_torch_on_added;
    block.on_neighbor_changed = block_torch_on_neighbor_changed;
    block.clip = block_torch_clip;
    block.visual_update = block_torch_visual_update;

    block_list[block.id] = block;

    return block;
}

AABB_t block_torch_get_collision_aabb(block_t *block, int x, int y, int z) {
    return (AABB_t){ .null = 1 };
}

uint8_t block_torch_can_place_at(block_t *block, world_t *world, int x, int y, int z) {
    return world_is_solid((world_t *)world, x, y - 1, z) ||
           world_is_solid((world_t *)world, x + 1, y, z) ||
           world_is_solid((world_t *)world, x - 1, y, z) ||
           world_is_solid((world_t *)world, x, y, z + 1) ||
           world_is_solid((world_t *)world, x, y, z - 1);
}

void block_torch_on_placed(block_t *block, world_t *world, int x, int y, int z, uint8_t side) {
    uint8_t metadata = world_get_block_metadata(world, x, y, z);
    if(side == 1 && world_is_solid(world, x, y - 1, z)) {
        metadata = 5;
    }

    if(side == 2 && world_is_solid(world, x, y, z + 1)) {
        metadata = 4;
    }

    if(side == 3 && world_is_solid(world, x, y, z - 1)) {
        metadata = 3;
    }

    if(side == 4 && world_is_solid(world, x + 1, y, z)) {
        metadata = 2;
    }

    if(side == 5 && world_is_solid(world, x - 1, y, z)) {
        metadata = 1;
    }

    world_set_block_metadata(world, x, y, z, metadata);
}

void block_torch_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    block_update(block, world, x, y, z, random);
    if(world_get_block_metadata(world, x, y, z) == 0) {
        block->on_added(block, world, x, y, z);
    }
}

uint8_t private_drop_torch(block_t *block, world_t *world, int x, int y, int z) {
    if(!block->can_place_at(block, world, x, y, z)) {
        block_spawn_items(block, world, x, y, z, world_get_block_metadata(world, x, y, z));
        world_set_block_with_update(world, x, y, z, 0);
        return 0;
    }
    return 1;
}

void block_torch_on_added(block_t *block, world_t *world, int x, int y, int z) {
    if(world_is_solid(world, x - 1, y, z)) {
        world_set_block_metadata(world, x, y, z, 1);
    }else if(world_is_solid(world, x + 1, y, z)) {
        world_set_block_metadata(world, x, y, z, 2);
    }else if(world_is_solid(world, x, y, z - 1)) {
        world_set_block_metadata(world, x, y, z, 3);
    }else if(world_is_solid(world, x, y, z + 1)) {
        world_set_block_metadata(world, x, y, z, 4);
    }else if(world_is_solid(world, x, y - 1, z)) {
        world_set_block_metadata(world, x, y, z, 5);
    }

    private_drop_torch(block, world, x, y, z);
}

void block_torch_on_neighbor_changed(block_t *block, world_t *world, int x, int y, int z, uint8_t block_id) {
    if(private_drop_torch(block, world, x, y, z)) {
        uint8_t metadata = world_get_block_metadata(world, x, y, z);
        uint8_t cant_stay = 0;
        if(!world_is_solid(world, x - 1, y, z) && metadata == 1) {
            cant_stay = 1;
        }else if(!world_is_solid(world, x + 1, y, z) && metadata == 2) {
            cant_stay = 1;
        }else if(!world_is_solid(world, x, y, z - 1) && metadata == 3) {
            cant_stay = 1;
        }else if(!world_is_solid(world, x, y, z + 1) && metadata == 4) {
            cant_stay = 1;
        }else if(!world_is_solid(world, x, y - 1, z) && metadata == 5) {
            cant_stay = 1;
        }

        if(cant_stay) {
            block_spawn_items(block, world, x, y, z, world_get_block_metadata(world, x, y, z));
            world_set_block_with_update(world, x, y, z, 0);
        }
    }
}

hit_result_t block_torch_clip(block_t *block, world_t *world, int x, int y, int z, vec3_t v0, vec3_t v1) {
    uint8_t metadata = world_get_block_metadata(world, x, y, z);
    if(metadata == 1) {
        block_set_bounds(block, 0.0, 0.2, 0.35, 0.3, 0.8, 0.65);
    }else if(metadata == 2) {
        block_set_bounds(block, 0.7, 0.2, 0.35, 1.0, 0.8, 0.65);
    }else if(metadata == 3) {
        block_set_bounds(block, 0.35, 0.2, 0.0, 0.65, 0.8, 0.3);
    }else if(metadata == 4) {
        block_set_bounds(block, 0.35, 0.2, 0.7, 0.65, 0.8, 1.0);
    }else {
        block_set_bounds(block, 0.4, 0.0, 0.4, 0.6, 0.6, 0.6);
    }

    return block_clip(block, world, x, y, z, v0, v1);
}

void block_torch_visual_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    uint8_t metadata = world_get_block_metadata(world, x, y, z);
    float px = x + 0.5;
    float py = y + 0.7;
    float pz = z + 0.5;
    if(metadata == 1) {
        px -= 0.27;
        py += 0.22;
    }else if(metadata == 2) {
        px += 0.27;
        py += 0.22;
    }else if(metadata == 3) {
        py += 0.22;
        pz -= 0.27;
    }else if(metadata == 4) {
        py += 0.22;
        pz += 0.27;
    }

    world_spawn_particle(world, PARTICLE_SMOKE, px, py, pz, 0.0, 0.0, 0.0);
    world_spawn_particle(world, PARTICLE_FLAME, px, py, pz, 0.0, 0.0, 0.0);
}
