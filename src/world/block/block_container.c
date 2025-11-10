#include <world/block/block_container.h>

#include <world/material/material.h>
#include <world/world.h>

static uint8_t block_container_keep_inventory = 0;

void block_container_set_keep_inventory(uint8_t keep_inventory) {
    block_container_keep_inventory = keep_inventory;
}

block_t block_container_create(uint8_t id, int texture_id, block_sound_t *sound, float hardness, float resistance, material_t *material) {
    block_t block = block_create(id, texture_id, sound, hardness, resistance, material);

    block.on_added = block_container_on_added;
    block.on_removed = block_container_on_removed;
    block.is_container = 1;

    block_list[block.id] = block;

    return block;
}

void block_container_on_added(block_t *block, world_t *world, int x, int y, int z) {
    block_on_added(block, world, x, y, z);
    if(block_container_keep_inventory) return;
    world_set_tile_entity(world, x, y, z, block->get_tile_entity(world));
}

void block_container_on_removed(block_t *block, world_t *world, int x, int y, int z) {
    block_on_removed(block, world, x, y, z);
    if(block_container_keep_inventory) return;
    world_remove_tile_entity(world, x, y, z);
}