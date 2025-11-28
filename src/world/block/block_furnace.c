#include <world/block/block_furnace.h>

#include <world/block/block_container.h>
#include <world/block/blocks.h>
#include <world/world.h>

#include <stdlib.h>

block_t block_furnace_create(uint8_t block_id, uint8_t is_lit) {
    block_t block = block_container_create(block_id, TEXTURE_FURNACE, &block_sounds.stone, 3.5, 0, &materials.rock);

    block.furnace_active = is_lit;

    block.on_added = block_furnace_on_added;
    block.get_texture = block_furnace_get_texture;
    block.visual_update = block_furnace_visual_update;
    block.get_texture_side = block_furnace_get_texture_side;
    block.on_interacted = block_furnace_on_interacted;
    block.get_tile_entity = block_furnace_get_tile_entity;

    block_list[block.id] = block;

    return block;
}

void private_set_default_direction(block_t *block, world_t *world, int x, int y, int z) {
    uint8_t block_north = world_get_block(world, x, y, z - 1);
    uint8_t block_south = world_get_block(world, x, y, z + 1);
    uint8_t block_east = world_get_block(world, x - 1, y, z);
    uint8_t block_west = world_get_block(world, x + 1, y, z);
    uint8_t direction = 3;

    if(block_list[block_north].is_opaque && !block_list[block_south].is_opaque) {
        direction = 3;
    }

    if(block_list[block_south].is_opaque && !block_list[block_north].is_opaque) {
        direction = 2;
    }

    if(block_list[block_east].is_opaque && !block_list[block_west].is_opaque) {
        direction = 5;
    }

    if(block_list[block_west].is_opaque && !block_list[block_east].is_opaque) {
        direction = 4;
    }

    world_set_block_metadata(world, x, y, z, direction);
}

void block_furnace_on_added(block_t *block, world_t *world, int x, int y, int z) {
    block_container_on_added(block, world, x, y, z);
    private_set_default_direction(block, world, x, y, z);
}

int block_furnace_get_texture(block_t *block, world_t *world, int x, int y, int z, uint8_t face) {
    if(face == 1) {
        return blocks.stone.texture_id;
    }
    if(face == 0) {
        return blocks.stone.texture_id;
    }
    uint8_t metadata = world_get_block_metadata(world, x, y, z);
    if(metadata == 0) {
        private_set_default_direction(block, world, x, y, z);
        metadata = world_get_block_metadata(world, x, y, z);
    }

    return face != metadata ? block->texture_id : (block->furnace_active ? block->texture_id + 16 : block->texture_id - 1);
}

void block_furnace_visual_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    if(!block->furnace_active) return;
    uint8_t metadata = world_get_block_metadata(world, x, y, z);
    float px = x + 0.5;
    float py = y + random_next_double(random) * 6.0 / 16.0;
    float pz = z + 0.5;
    float xz_offset = random_next_double(random) * 0.6 - 3.0;
    if(metadata == 4) {
        world_spawn_particle(world, PARTICLE_SMOKE, px - 0.52, py, pz + xz_offset, 0, 0, 0);
        world_spawn_particle(world, PARTICLE_FLAME, px - 0.52, py, pz + xz_offset, 0, 0, 0);
    }
    if(metadata == 5) {
        world_spawn_particle(world, PARTICLE_SMOKE, px + 0.52, py, pz + xz_offset, 0, 0, 0);
        world_spawn_particle(world, PARTICLE_FLAME, px + 0.52, py, pz + xz_offset, 0, 0, 0);
    }
    if(metadata == 2) {
        world_spawn_particle(world, PARTICLE_SMOKE, px + xz_offset, py, pz - 0.52, 0, 0, 0);
        world_spawn_particle(world, PARTICLE_FLAME, px + xz_offset, py, pz - 0.52, 0, 0, 0);
    }
    if(metadata == 3) {
        world_spawn_particle(world, PARTICLE_SMOKE, px + xz_offset, py, pz + 0.52, 0, 0, 0);
        world_spawn_particle(world, PARTICLE_FLAME, px + xz_offset, py, pz + 0.52, 0, 0, 0);
    }
}

int block_furnace_get_texture_side(block_t *block, uint8_t face) {
    return face == 1 ? blocks.stone.texture_id : (face == 0 ? blocks.stone.texture_id : (face == 3 ? block->texture_id - 1 : block->texture_id));
}

uint8_t block_furnace_on_interacted(block_t *block, world_t *world, int x, int y, int z, entity_t *player) {
    tile_entity_t *tile_entity = world_get_tile_entity(world, x, y, z);
    if(tile_entity == NULL) {
        return 1;
    }

    tile_entity->inventory.host_tile_entity = tile_entity;
    player_render_furnace_screen(player, tile_entity);
    return 1;
}

tile_entity_t *block_furnace_get_tile_entity(world_t *world)  {
    tile_entity_t *tile_entity = malloc(sizeof(tile_entity_t));
    tile_entity_furnace_create(tile_entity, world, 0, 0, 0);

    return tile_entity;
}