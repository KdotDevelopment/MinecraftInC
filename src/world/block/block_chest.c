#include <world/block/block_chest.h>

#include <entity/entity_item.h>
#include <gui/container/inventory_double_chest.h>
#include <item/item_stack.h>
#include <world/block/block_container.h>
#include <world/block/blocks.h>
#include <world/block/tile_entity/tile_entity_chest.h>
#include <world/material/materials.h>
#include <world/world.h>

#include <stdlib.h>

block_t block_chest_create() {
    block_t block = block_container_create(BLOCK_CHEST, TEXTURE_CHEST, &block_sounds.wood, 2.5, 0, &materials.wood);
    
    block.get_texture = block_chest_get_texture;
    block.get_texture_side = block_chest_get_texture_side;
    block.can_place_at = block_chest_can_place_at;
    block.on_removed = block_chest_on_removed;
    block.on_interacted = block_chest_on_interacted;
    block.get_tile_entity = block_chest_get_tile_entity;

    block_list[block.id] = block;

    return block;
}

int block_chest_get_texture(block_t *block, world_t *world, int x, int y, int z, uint8_t side) {
    if(side == 1) return block->texture_id - 1;
    if(side == 0) return block->texture_id - 1;

    uint8_t block_north = world_get_block(world, x, y, z - 1);
    uint8_t block_south = world_get_block(world, x, y, z + 1);
    uint8_t block_west = world_get_block(world, x - 1, y, z);
    uint8_t block_east = world_get_block(world, x + 1, y, z);

    if(block_north != block->id && block_south != block->id) {
        if(block_west != block->id && block_east != block->id) {
            uint8_t preferred_side = 3;

            if(block_list[block_north].is_opaque && !block_list[block_south].is_opaque) {
                preferred_side = 3;
            }

            if(block_list[block_south].is_opaque && !block_list[block_north].is_opaque) {
                preferred_side = 2;
            }

            if(block_list[block_west].is_opaque && !block_list[block_east].is_opaque) {
                preferred_side = 5;
            }

            if(block_list[block_east].is_opaque && !block_list[block_west].is_opaque) {
                preferred_side = 4;
            }

            return side == preferred_side ? block->texture_id + 1 : block->texture_id;
        }else if(side != 4 && side != 5) {
            int texture_offset = 0;

            if(block_west == block->id) {
                texture_offset = -1;
            }

            int diagonal_north = world_get_block(world, block_west == block->id ? x - 1 : x + 1, y, z - 1);
            int diagonal_south = world_get_block(world, block_west == block->id ? x - 1 : x + 1, y, z + 1);

            if(side == 3) {
                texture_offset = -1 - texture_offset;
            }

            uint8_t preferred_side = 3;
            if((block_list[block_north].is_opaque || block_list[diagonal_north].is_opaque) 
               && !block_list[block_south].is_opaque && !block_list[diagonal_south].is_opaque) {
                preferred_side = 3;
            }

            if((block_list[block_south].is_opaque || block_list[diagonal_south].is_opaque) 
               && !block_list[block_north].is_opaque && !block_list[diagonal_north].is_opaque) {
                preferred_side = 2;
            }

            return (side == preferred_side ? block->texture_id + 16 : block->texture_id + 32) + texture_offset;
        }else {
            return block->texture_id;
        }
    }else if(side != 2 && side != 3) {
        int texture_offset = 0;

        if(block_north == block->id) {
            texture_offset = -1;
        }

        int diagonal_west = world_get_block(world, x - 1, y, block_north == block->id ? z - 1 : z + 1);
        int diagonal_east = world_get_block(world, x + 1, y, block_north == block->id ? z - 1 : z + 1);

        if(side == 4) {
            texture_offset = -1 - texture_offset;
        }

        uint8_t preferred_side = 5;
        if((block_list[block_north].is_opaque || block_list[diagonal_west].is_opaque) 
            && !block_list[block_south].is_opaque && !block_list[diagonal_east].is_opaque) {
            preferred_side = 5;
        }

        if((block_list[block_south].is_opaque || block_list[diagonal_east].is_opaque) 
            && !block_list[block_north].is_opaque && !block_list[diagonal_west].is_opaque) {
            preferred_side = 4;
        }

        return (side == preferred_side ? block->texture_id + 16 : block->texture_id + 32) + texture_offset;
    }else {
        return block->texture_id;
    }
}

int block_chest_get_texture_side(block_t *block, uint8_t side) {
    return side == 1 ? block->texture_id - 1 : 
        (side == 0 ? block->texture_id - 1 : 
        (side == 3 ? block->texture_id + 1 : 
        block->texture_id));
}

uint8_t private_has_neighboring_chest(block_t *block, world_t *world, int x, int y, int z) {
    if(world_get_block(world, x, y, z) != block->id) return 0;

    return world_get_block(world, x - 1, y, z) == block->id ||
           world_get_block(world, x + 1, y, z) == block->id ||
           world_get_block(world, x, y, z - 1) == block->id ||
           world_get_block(world, x, y, z + 1) == block->id;
}

uint8_t block_chest_can_place_at(block_t *block, world_t *world, int x, int y, int z) {
    int chest_count = 0;

    if(world_get_block(world, x - 1, y, z) == block->id) {
        chest_count++;
    }
    if(world_get_block(world, x + 1, y, z) == block->id) {
        chest_count++;
    }
    if(world_get_block(world, x, y, z - 1) == block->id) {
        chest_count++;
    }
    if(world_get_block(world, x, y, z + 1) == block->id) {
        chest_count++;
    }

    return chest_count > 1 ? 0 :
        (private_has_neighboring_chest(block, world, x - 1, y, z) ? 0 :
        (private_has_neighboring_chest(block, world, x + 1, y, z) ? 0 :
        (private_has_neighboring_chest(block, world, x, y, z - 1) ? 0 :
        !private_has_neighboring_chest(block, world, x, y, z + 1))));
}

void block_chest_on_removed(block_t *block, world_t *world, int x, int y, int z) {
    tile_entity_t *tile_entity = world_get_tile_entity(world, x, y, z);

    for(int i = 0; i < tile_entity->inventory_size; i++) {
        item_stack_t item_stack = tile_entity_chest_get_item(&tile_entity->inventory, i);
        if(item_stack.item_id != 0) {
            float rand_x = random_next_double(&world->random) * 0.8 + 0.1;
            float rand_y = random_next_double(&world->random) * 0.8 + 0.1;
            float rand_z = random_next_double(&world->random) * 0.8 + 0.1;

            while(item_stack.stack_size > 0) {
                int item_count = random_next_int_range(&world->random, 21) + 10;
                if(item_count > item_stack.stack_size) {
                    item_count = item_stack.stack_size;
                }

                item_stack.stack_size -= item_count;
                entity_t *item = malloc(sizeof(entity_t));
                item_stack_t drop_stack = item_stack_create(item_stack.item_id, item_count, item_stack.item_damage);
                entity_item_create(item, world, x + rand_x, y + rand_y, z + rand_z, drop_stack);
                item->xd = random_next_normal(&world->random, 1.0) * 0.05;
                item->yd = random_next_normal(&world->random, 1.0) * 0.05 + 0.2;
                item->zd = random_next_normal(&world->random, 1.0) * 0.05;
                world_spawn_entity(world, item);
            }
        }
    }

    block_container_on_removed(block, world, x, y, z);
}

uint8_t block_chest_on_interacted(block_t *block, world_t *world, int x, int y, int z, entity_t *player) {
    tile_entity_t *tile_entity = world_get_tile_entity(world, x, y, z);
    if(world_is_solid(world, x, y + 1, z)) return 1;
    else if(world_get_block(world, x - 1, y, z) == block->id && world_is_solid(world, x - 1, y + 1, z)) return 1;
    else if(world_get_block(world, x + 1, y, z) == block->id && world_is_solid(world, x + 1, y + 1, z)) return 1;
    else if(world_get_block(world, x, y, z - 1) == block->id && world_is_solid(world, x, y + 1, z - 1)) return 1;
    else if(world_get_block(world, x, y, z + 1) == block->id && world_is_solid(world, x, y + 1, z + 1)) return 1;

    tile_entity->chest_inventory = tile_entity->inventory;
    tile_entity->chest_inventory.host_tile_entity = tile_entity;
    tile_entity->chest_inventory.upper_chest = NULL;
    tile_entity->chest_inventory.lower_chest = NULL;

    if(world_get_block(world, x - 1, y, z) == block->id) {
        tile_entity_t *neighbor = world_get_tile_entity(world, x - 1, y, z);
        tile_entity->chest_inventory = inventory_double_chest_create("Large chest", &neighbor->inventory, &tile_entity->inventory);
        tile_entity->chest_inventory.host_tile_entity = tile_entity;
    }

    if(world_get_block(world, x + 1, y, z) == block->id) {
        tile_entity_t *neighbor = world_get_tile_entity(world, x + 1, y, z);
        tile_entity->chest_inventory = inventory_double_chest_create("Large chest", &tile_entity->inventory, &neighbor->inventory);
        tile_entity->chest_inventory.host_tile_entity = tile_entity;
    }

    if(world_get_block(world, x, y, z - 1) == block->id) {
        tile_entity_t *neighbor = world_get_tile_entity(world, x, y, z - 1);
        tile_entity->chest_inventory = inventory_double_chest_create("Large chest", &neighbor->inventory, &tile_entity->inventory);
        tile_entity->chest_inventory.host_tile_entity = tile_entity;
    }

    if(world_get_block(world, x, y, z + 1) == block->id) {
        tile_entity_t *neighbor = world_get_tile_entity(world, x, y, z + 1);
        tile_entity->chest_inventory = inventory_double_chest_create("Large chest", &tile_entity->inventory, &neighbor->inventory);
        tile_entity->chest_inventory.host_tile_entity = tile_entity;
    }

    player_render_chest_screen(player, tile_entity);
    return 1;
}

tile_entity_t *block_chest_get_tile_entity(world_t *world) {
    tile_entity_t *tile_entity = malloc(sizeof(tile_entity_t));
    tile_entity_chest_create(tile_entity, world, 0, 0, 0);

    return tile_entity;
}