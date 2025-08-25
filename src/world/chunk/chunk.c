#include <world/chunk/chunk.h>

#include <util/array_list.h>
#include <util/math_helper.h>
#include <world/chunk/nibble_array.h>

#include <stdio.h>

void chunk_create(chunk_t *chunk, world_t *world, int x, int z) {
    memset(chunk, 0, sizeof(chunk_t));

    chunk->world = world;
    chunk->x_pos = x;
    chunk->z_pos = z;

    for(int i = 0; i < 8; i++) {
        chunk->entities[i] = array_list_create(sizeof(uint64_t));
    }
}

void chunk_create_from(chunk_t *chunk, world_t *world, uint8_t *data, int x, int z) {
    chunk_create(chunk, world, x, z);

    memcpy(chunk->blocks, data, CHUNK_SIZE_WIDTH * CHUNK_SIZE_WIDTH * CHUNK_SIZE_HEIGHT);
    chunk->data = nibble_array_create(CHUNK_SIZE_WIDTH * CHUNK_SIZE_HEIGHT * CHUNK_SIZE_WIDTH);
    chunk->sky_light_map = nibble_array_create(CHUNK_SIZE_WIDTH * CHUNK_SIZE_HEIGHT * CHUNK_SIZE_WIDTH);
    chunk->block_light_map = nibble_array_create(CHUNK_SIZE_WIDTH * CHUNK_SIZE_HEIGHT * CHUNK_SIZE_WIDTH);
}

int chunk_get_height_value(chunk_t *chunk, int x, int z) {
    return chunk->height_map[z * CHUNK_SIZE_WIDTH + x] & 0xFF;
}

void chunk_generate_height_map(chunk_t *chunk) {
    int height = 127;
    for(int x = 0; x < CHUNK_SIZE_WIDTH; x++) {
        for(int z = 0; z < CHUNK_SIZE_WIDTH; z++) {
            chunk->height_map[z * CHUNK_SIZE_WIDTH + x] = -128;
            chunk_relight_block(chunk, x, CHUNK_SIZE_HEIGHT, z);
            if((chunk->height_map[z * CHUNK_SIZE_WIDTH + x] & 0xFF) < height) {
                height = chunk->height_map[z * CHUNK_SIZE_WIDTH + x] & 0xFF;
            }
        }
    }

    chunk->lowest_block_height = height;

    for(int x = 0; x < CHUNK_SIZE_WIDTH; x++) {
        for(int z = 0; z < CHUNK_SIZE_WIDTH; z++) {
            chunk_update_skylight(chunk, x, z);
        }
    }

    chunk->is_modified = 1;
}

void chunk_update_skylight(chunk_t *chunk, int x, int z) {
    int height = chunk_get_height_value(chunk, x, z);
    x += chunk->x_pos * CHUNK_SIZE_WIDTH;
    z += chunk->z_pos * CHUNK_SIZE_WIDTH;
    chunk_check_skylight_neighbor_height(chunk, x - 1, z, height);
    chunk_check_skylight_neighbor_height(chunk, x + 1, z, height);
    chunk_check_skylight_neighbor_height(chunk, x, z - 1, height);
    chunk_check_skylight_neighbor_height(chunk, x, z + 1, height);
}

void chunk_check_skylight_neighbor_height(chunk_t *chunk, int x, int z, int height) {
    int neighbor_height = chunk_get_height_value(chunk, x, z);
    if (neighbor_height > height) {
        //world_schedule_lighting_update(chunk->world, LIGHT_TYPE_SKY, x, height, z, x, neighbor_height, z);
    }else if(neighbor_height < height) {
        //world_schedule_lighting_update(chunk->world, LIGHT_TYPE_SKY, x, neighbor_height, z, x, height, z);
    }

    chunk->is_modified = 1;
}

void chunk_relight_block(chunk_t *chunk, int x, int y, int z) {
    int height = chunk_get_height_value(chunk, x, z);
    int height_index = height;
    if(y > height) height_index = y;

    while(height_index > 0 && block_list[chunk_get_block_id(chunk, x, height_index - 1, z)].light_opacity == 0) {
        height_index--;
    }

    if(height_index != height) {
        //world_mark_blocks_dirty_vertical(chunk->world, x, z, height_index, height);
        chunk->height_map[z * CHUNK_SIZE_WIDTH + x] = (uint8_t)height_index;
        if(height_index < chunk->lowest_block_height) {
            chunk->lowest_block_height = height_index;
        }else {
            y = CHUNK_SIZE_HEIGHT;

            for(int xx = 0; xx < CHUNK_SIZE_WIDTH; xx++) {
                for(int zz = 0; zz < CHUNK_SIZE_WIDTH; zz++) {
                    if((chunk->height_map[zz * CHUNK_SIZE_WIDTH + xx] & 0xFF) < y) {
                        y = chunk->height_map[zz * CHUNK_SIZE_WIDTH + xx] & 0xFF;
                    }
                }
            }

            chunk->lowest_block_height = y;
        }

        int xx = (chunk->x_pos * CHUNK_SIZE_WIDTH) + x;
        int zz = (chunk->z_pos * CHUNK_SIZE_WIDTH) + z;
        if(height_index < height) {
            for(int i = height_index; i < height; i++) {
                nibble_array_set(chunk->sky_light_map, x, i, z, 15);
            }
        }else {
            //world_schedule_lighting_update(chunk->world, LIGHT_TYPE_SKY, xx, height, zz, xx, height_index, zz);

            for(int i = height; i < height_index; i++) {
                nibble_array_set(chunk->sky_light_map, x, i, z, 0);
            }
        }

        uint8_t light_level = 15;

        for(height = height_index; height_index > 0 && light_level > 0; nibble_array_set(chunk->sky_light_map, x, height_index, z, light_level)) {
            height_index--;
            int opacity = block_list[chunk_get_block_id(chunk, x, height_index, z)].light_opacity;
            if(opacity == 0) opacity = 1;
            light_level -= opacity;
            if(light_level < 0) light_level = 0;
        }

        while(height_index > 0 && block_list[chunk_get_block_id(chunk, x, height_index, z)].light_opacity == 0) {
            height_index--;
        }

        if(height_index != height) {
            //world_schedule_lighting_update(chunk->world, LIGHT_TYPE_SKY, xx - 1, height_index, zz - 1, xx + 1, height, zz);
        }

        chunk->is_modified = 1;
    }
}

uint8_t chunk_get_block_id(chunk_t *chunk, int x, int y, int z) {
    return chunk->blocks[x << 11 | z << 7 | y];
}

uint8_t chunk_set_block(chunk_t *chunk, int x, int y, int z, int block_id) {
    int height = chunk_get_height_value(chunk, x, z) & 0xFF;
    uint8_t old_block_id = chunk_get_block_id(chunk, x, y, z);
    if(old_block_id == block_id) return 0;

    int xx = (chunk->x_pos * CHUNK_SIZE_WIDTH) + x;
    int zz = (chunk->z_pos * CHUNK_SIZE_WIDTH) + z;
    if(old_block_id != 0) {
        block_list[old_block_id].on_removed(&block_list[old_block_id], chunk->world, xx, y, zz);
    }

    chunk->blocks[x << 11 | z << 7 | y] = block_id;
    nibble_array_set(chunk->data, x, y, z, 0);
    if(block_list[block_id].light_opacity != 0) {
        if(y >= height) {
            chunk_relight_block(chunk, x, y + 1, z);
        }
    }else if (y == height - 1) {
        chunk_relight_block(chunk, x, y, z);
    }

    //world_schedule_lighting_update(chunk->world, LIGHT_TYPE_SKY, xx, y, zz, xx, y, zz);
    //world_schedule_lighting_update(chunk->world, LIGHT_TYPE_BLOCK, xx, y, zz, xx, y, zz);
    chunk_update_skylight(chunk, x, z);

    if(block_id != 0) {
        block_list[block_id].on_added(&block_list[block_id], chunk->world, xx, y, zz);
    }

    chunk->is_modified = 1;
    return 1;
}

uint8_t chunk_get_block_metadata(chunk_t *chunk, int x, int y, int z) {
    return nibble_array_get(chunk->data, x, y, z);
}

void chunk_set_block_metadata(chunk_t *chunk, int x, int y, int z, int data) {
    chunk->is_modified = 1;
    nibble_array_set(chunk->data, x, y, z, data);
}

uint8_t chunk_get_saved_light_value(chunk_t *chunk, uint8_t light_type, int x, int y, int z) {
    return light_type == LIGHT_TYPE_SKY ? nibble_array_get(chunk->sky_light_map, x, y, z) : (light_type == LIGHT_TYPE_BLOCK ? nibble_array_get(chunk->block_light_map, x, y, z) : 0);
}

void chunk_set_light_value(chunk_t *chunk, uint8_t light_type, int x, int y, int z, uint8_t level) {
    chunk->is_modified = 1;
    if(light_type == LIGHT_TYPE_SKY) {
        nibble_array_set(chunk->sky_light_map, x, y, z, level);
    }else if(light_type == LIGHT_TYPE_BLOCK) {
        nibble_array_set(chunk->block_light_map, x, y, z, level);
    }
}

// I'm assuming this is time-of-day related
uint8_t chunk_get_block_light_value(chunk_t *chunk, int x, int y, int z, uint8_t time_factor) {
    uint8_t sky = nibble_array_get(chunk->sky_light_map, x, y, z);
    if(sky > 0) chunk->is_lit = 1;
    sky -= time_factor;
    uint8_t block = nibble_array_get(chunk->block_light_map, x, y, z);
    if(block > sky) sky = block;

    return sky;
}

void chunk_add_entity(chunk_t *chunk, entity_t *entity) {
    int x = floor_double(entity->x / CHUNK_SIZE_WIDTH);
    int z = floor_double(entity->z / CHUNK_SIZE_WIDTH);
    if(x != chunk->x_pos || z != chunk->z_pos) {
        printf("Wrong location!\n");
    }

    int y = floor_double(entity->y / CHUNK_SIZE_WIDTH);
    if(y < 0) y = 0;

    if(y >= CHUNK_SIZE_HEIGHT / CHUNK_SIZE_WIDTH) y = CHUNK_SIZE_HEIGHT / CHUNK_SIZE_WIDTH - 1;

    // note to self: we are storing the pointer in these arrays (as they have been malloc'd before)
    // this pointer to a local variable gets dereferenced within the function, storing the entity pointer
    uint64_t ptr = *(uint64_t *)entity; // this might be unnecessary but its easier for me to understand
    array_list_push(chunk->entities[y], &ptr);
    chunk->is_modified = 1;
}

void chunk_remove_entity_index(chunk_t *chunk, entity_t *entity, int index) {
    if(index < 0) index = 0;
    if(index >= CHUNK_SIZE_HEIGHT / CHUNK_SIZE_WIDTH) index = CHUNK_SIZE_HEIGHT / CHUNK_SIZE_WIDTH - 1;
    if(!array_list_contains(chunk->entities[index], entity)) printf("There\'s no such entity to remove: %d\n", index);

    int array_index = array_list_index_of(chunk->entities[index], entity);
    array_list_remove(chunk->entities[index], array_index);
    chunk->is_modified = 1;
}

uint8_t chunk_can_block_see_sky(chunk_t *chunk, int x, int y, int z) {
    return y >= (chunk_get_height_value(chunk, x, z) & 0xFF);
}

uint8_t chunk_needs_saving(chunk_t *chunk, uint8_t check_entities) {
    if(chunk->is_modified) return 1;
    if(check_entities) {
        if(chunk->has_entities) return 1;
        for(int i = 0; i < CHUNK_SIZE_HEIGHT / CHUNK_SIZE_WIDTH; i++) {
            if(array_list_length(chunk->entities[i]) > 0) return 1;
        }
    }
    return 0;
}