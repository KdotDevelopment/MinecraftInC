#include <world/world.h>
#include <renderer/world_renderer.h>
#include <world/block/block.h>
#include <world/block/blocks.h>
#include <sound/sounds.h>
#include <minecraft.h>

#include <util/array_list.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void world_create(world_t *world, struct minecraft_s *minecraft, progress_bar_t *progress_bar, int size) {
    //world_t world = { 0 };
    
    world->random = random_create(time(NULL));
    world->minecraft = minecraft;
    world->sky_color = 0x99CCFFFF;
    world->fog_color = 0xB0D0FFFF;
    world->cloud_color = 0xFFFFFFFF;
    world->unprocessed = 0;
    world->tick_count = 0;
    world->tick_list = array_list_create(sizeof(next_tick_data_t));
    world->random = random_create(time(NULL));
    world->random_value = (int)random_next_int(&world->random);
    world->generator = world_gen_create();
    world->progress_bar = progress_bar;
    int block_count = 128 << size;
    world_gen_generate(&world->generator, block_count, block_count, (struct world_s *)world);

    //return world;
}

void world_regenerate(world_t *world, int size) {
    world_destroy(world);
    world_create(world, world->minecraft, world->progress_bar, size);
}

void world_set_data(world_t *world, int w, int d, int h, uint8_t *blocks) {
    if(world->blocks) free(world->blocks);
    world->blocks = NULL;
    if(world->light_depths) free(world->light_depths);
    world->light_depths = NULL;
    world->width = w;
    world->height = h;
    world->depth = d;
    entity_map_create(&world->entity_map, world->width, world->depth, world->height);
    world->blocks = calloc(w * d * h, 1);
    memcpy(world->blocks, blocks, w * d * h);
    world->light_depths = calloc(w * h, sizeof(int));
    for(int i = 0; i < w * h; i++) {
        world->light_depths[i] = world->depth - 1;
    }
    world_calculate_light_depths(world, 0, 0, w, h);
    for (int i = 0; i < array_list_length(world->tick_list); i++) {
        next_tick_data_t *tick = &world->tick_list[i];
        free(tick);
    }
    world->tick_list = array_list_clear(world->tick_list);
    world_find_spawn(world);
}

void world_find_spawn(world_t *world) {
    int i = 0;
    int x;
    int y;
    int z;
    do {
        i++;
        x = rand() % ((world->width / 2) - (world->width / 4)) + (world->width / 4);
        z = rand() % ((world->height / 2) - (world->height / 4)) + (world->height / 4);
        y = world_get_highest_block(world, x, z) + 1;
        if(i == 10000) {
            world->spawn_x = x;
            world->spawn_y = -100;
            world->spawn_z = z;
            return;
        }
    }while(y <= world_get_water_world(world));
    world->spawn_x = x;
    world->spawn_y = y;
    world->spawn_z = z;
}

void world_calculate_light_depths(world_t *world, int x0, int y0, int x1, int y1) {
    for(int i = x0; i < x0 + x1; i++) {
        for(int j = y0; j < y0 + y1; j++) {
            int depth = world->light_depths[i + j * world->width];
            int k = world->depth - 1;
            while(!world_is_light_blocker(world, i, k, j) && k > 0) k--;
            world->light_depths[i + j * world->width] = k;
            if(depth != k) {
                int min = depth < k ? depth : k;
                int max = depth > k ? depth : k;
                if(world->renderer != NULL) world_renderer_queue_chunks((world_renderer_t *)world->renderer, i - 1, min - 1, j - 1, i + 1, max + 1, j + 1);
            }
        }
    }
}

uint8_t world_is_light_blocker(world_t *world, int x, int y, int z) {
    block_t *block = &block_list[world_get_block(world, x, y, z)];
    return block->id == blocks.air.id ? 0 : block->is_opaque;
}

AABB_t *world_get_cubes(world_t *world, AABB_t box) {
    AABB_t *list = array_list_create(sizeof(AABB_t));
    int x0 = box.x0;
    int y0 = box.y0;
    int z0 = box.z0;
    int x1 = box.x1 + 1;
    int y1 = box.y1 + 1;
    int z1 = box.z1 + 1;
    if(box.x0 < 0) x0--;
    if(box.y0 < 0) y0--;
    if(box.z0 < 0) z0--;
    for(int i = x0; i < x1; i++) {
        for(int j = y0; j < y1; j++) {
            for(int k = z0; k < z1; k++) {
                AABB_t bb = { 0 };
                if(i >= 0 && j >= 0 && k >= 0 && i < world->width && j < world->depth && k < world->height) {
                    block_t *block = &block_list[world_get_block(world, i, j, k)];
                    if (block->id != blocks.air.id) {
                        bb = block->get_collision_aabb(block, i, j, k);
                        if(!bb.null && AABB_intersects_inner(box, bb)) {
                            list = array_list_push(list, &bb);
                        }
                    }
                }else if(i < 0 || j < 0 || k < 0 || i >= world->width || k >= world->height) {
                    block_t *block = &block_list[blocks.bedrock.id];
                    AABB_t bb = block->get_collision_aabb(block, i, j, k);
                    if(!bb.null && AABB_intersects_inner(box, bb)) {
                        list = array_list_push(list, &bb);
                    }
                }
            }
        }
    }
    return list;
}

void world_swap(world_t *world, int x0, int y0, int z0, int x1, int y1, int z1) {
    uint8_t t1 = world_get_block(world, x0, y0, z0);
    uint8_t t2 = world_get_block(world, x1, y1, z1);
    world_set_block_no_neighbor_change(world, x0, y0, z0, t2);
    world_set_block_no_neighbor_change(world, x1, y1, z1, t1);
    world_update_neighbors_at(world, x0, y0, z0, t2);
    world_update_neighbors_at(world, x1, y1, z1, t1);
}

uint8_t world_set_block(world_t *world, int x, int y, int z, uint8_t block_id) {
    if(world_set_block_no_neighbor_change(world, x, y, z, block_id)) {
        world_update_neighbors_at(world, x, y, z, block_id);
        return 1;
    }
    return 0;
}

uint8_t world_set_block_no_neighbor_change(world_t *world, int x, int y, int z, uint8_t block_id) {
    if (x < 0 || y < 0 || z < 0 || x >= world->width || y >= world->depth || z >= world->height) return 0;
    int i = (y * world->height + z) * world->width + x;
    if(block_id == world->blocks[i]) return 0;

    if(block_id == blocks.air.id && (x == 0 || z == 0 || x == world->width - 1 || z == world->height - 1) && y >= world_get_ground_world(world) && y < world_get_water_world(world)) {
        block_id = blocks.water.id;
    }
    uint8_t prev = world->blocks[i];
    world->blocks[i] = block_id;
    block_t *block = &block_list[block_id];
    block_t *prev_block = &block_list[prev];
    if(prev != blocks.air.id) {
        prev_block->on_removed(prev_block, (struct world_s *)world, x, y, z);
    }
    if(block_id != blocks.air.id) {
        block->on_added(block, (struct world_s *)world, x, y, z);
    }
    world_calculate_light_depths(world, x, z, 1, 1);
    if(world->renderer != NULL) {
        world_renderer_queue_chunks((world_renderer_t *)world->renderer, x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
    }

    return 1;
}

void world_update_block(world_t *world, int x, int y, int z, uint8_t block_id) {
    if (x < 0 || y < 0 || z < 0 || x >= world->width || y >= world->depth || z >= world->height) return;
    block_t *block = &block_list[world->blocks[(y * world->height + z) * world->width + x]];
    if (block->id != blocks.air.id) { 
        block->on_neighbor_changed(block, (struct world_s *)world, x, y, z, block_id);
    }
}

void world_update_neighbors_at(world_t *world, int x, int y, int z, uint8_t block_id) {
    world_update_block(world, x - 1, y, z, block_id);
    world_update_block(world, x + 1, y, z, block_id);
    world_update_block(world, x, y - 1, z, block_id);
    world_update_block(world, x, y + 1, z, block_id);
    world_update_block(world, x, y, z - 1, block_id);
    world_update_block(world, x, y, z + 1, block_id);
}

uint8_t world_set_block_no_update(world_t *world, int x, int y, int z, uint8_t block_id) {
    if (x < 0 || y < 0 || z < 0 || x >= world->width || y >= world->depth || z >= world->height) return 0;
    int i = (y * world->height + z) * world->width + x;
    if (block_id == world->blocks[i]) return 0;
    world->blocks[i] = block_id;
    return 1;
}

uint8_t world_is_lit(world_t *world, int x, int y, int z) {
    return !(x < 0 || y < 0 || z < 0 || x >= world->width || y >= world->depth || z >= world->height) ? y >= world->light_depths[x + z * world->width] : 1;
}

uint8_t world_get_block(world_t *world, int x, int y, int z) {
    if(x < 0 || y < 0 || z < 0
        || x >= world->width
        || y >= world->depth
        || z >= world->height) {
        return 0;
    }

    return world->blocks[(y * world->height + z) * world->width + x];
}

uint8_t world_is_solid_block(world_t *world, int x, int y, int z) {
    block_t *block = &block_list[world_get_block(world, x, y, z)];
    return block->is_solid;
}

void world_tick_entities(world_t *world) {
    entity_map_tick_all(&world->entity_map);
}

uint8_t world_is_free(world_t *world, AABB_t bb) {
    entity_t ***entities = entity_map_get_entities(&world->entity_map, NULL, bb.x0, bb.y0, bb.z0, bb.x1, bb.y1, bb.z1);
    return array_list_length(*entities) == 0;
}

void world_tick(world_t *world) {
    world->tick_count++;
    int a = 1;
    int b = 1;
    for(b = 1; 1 << a < world->width; a++);
    while(1 << b < world->height) b++;

    if(world->tick_count % 5 == 0) {
        for(int i = 0; i < array_list_length(world->tick_list); i++) {
            next_tick_data_t next_tick = world->tick_list[0];
            world->tick_list = array_list_remove(world->tick_list, 0);
            if(next_tick.ticks > 0) {
                next_tick.ticks--;
                world->tick_list = array_list_push(world->tick_list, &next_tick);
            }else {
                uint8_t block_id = world->blocks[(next_tick.y * world->height + next_tick.z) * world->width + next_tick.x];
                block_t *block = &block_list[block_id];
                if(world_is_in_bounds(world, next_tick.x, next_tick.y, next_tick.z) && block_id == next_tick.type && block_id != blocks.air.id) {
                    if(block->should_tick) {
                        block->update(block, (struct world_s *)world, next_tick.x, next_tick.y, next_tick.z, &world->random);
                    }
                }
            }
        }
    }

    world->unprocessed += world->width * world->height * world->depth;
    int c = world->unprocessed / 200;
    world->unprocessed -= c * 200;

    for(int i = 0; i < c; i++) {
        world->random_value = world->random_value * 3 + 1013904223;
        int y = world->random_value >> 2;
        int x = y & (world->width - 1);
        int z = y >> a & (world->height - 1);
        y = y >> (a + b) & (world->depth - 1);
        uint8_t block_id = world->blocks[(y * world->height + z) * world->width + x];
        block_t *block = &block_list[block_id];
        if(block->should_tick) {
            block->update(block, (struct world_s *)world, x, y, z, &world->random);
        }
    }
}

uint8_t world_is_in_bounds(world_t *world, int x, int y, int z) {
    return !(x < 0 || y < 0 || z < 0 || x >= world->width || y >= world->depth || z >= world->height);  
}

float world_get_ground_world(world_t *world) {
    return world_get_water_world(world) - 2;
}

float world_get_water_world(world_t *world) {
    return world->water_world;
}

uint8_t world_contains_any_liquid(world_t *world, AABB_t box) {
    int x0 = box.x0, y0 = box.y0, z0 = box.z0;
    int x1 = box.x1 + 1, y1 = box.y1 + 1, z1 = box.z1 + 1;
    if(box.x0 < 0.0) x0--;
    if(box.y0 < 0.0) y0--;
    if(box.z0 < 0.0) z0--;
    if(x0 < 0) x0 = 0;
    if(y0 < 0) y0 = 0;
    if(z0 < 0) z0 = 0;
    if(x1 > world->width) x1 = world->width;
    if(y1 > world->depth) y1 = world->depth;
    if(z1 > world->height) z1 = world->height;
    for(int i = x0; i < x1; i++) {
        for(int j = y0; j < y1; j++) {
            for(int k = z0; k < z1; k++) {
                block_t *block = &block_list[world_get_block(world, i, j, k)];
                if (block->id != blocks.air.id && block->liquid_type != LIQUID_NONE) return 1;
            }
        }
    }
    return 0;
}

uint8_t world_contains_liquid(world_t *world, AABB_t box, uint8_t liquid_type) {
    int x0 = box.x0, y0 = box.y0, z0 = box.z0;
    int x1 = box.x1 + 1, y1 = box.y1 + 1, z1 = box.z1 + 1;
    if(box.x0 < 0.0) x0--;
    if(box.y0 < 0.0) y0--;
    if(box.z0 < 0.0) z0--;
    if(x0 < 0) x0 = 0;
    if(y0 < 0) y0 = 0;
    if(z0 < 0) z0 = 0;
    if(x1 > world->width) x1 = world->width;
    if(y1 > world->depth) y1 = world->depth;
    if(z1 > world->height) z1 = world->height;
    for(int i = x0; i < x1; i++) {
        for(int j = y0; j < y1; j++) {
            for(int k = z0; k < z1; k++) {
                block_t *block = &block_list[world_get_block(world, i, j, k)];
                if (block->id != blocks.air.id && block->liquid_type == liquid_type) return 1;
            }
        }
    }
    return 0;
}

void world_add_to_next_tick(world_t *world, int x, int y, int z, uint8_t block_id) {
    next_tick_data_t tick = { .x = x, .y = y, .z = z, .type = block_id };
    block_t *block = &block_list[block_id];
    if(block_id != blocks.air.id) tick.ticks = block->tick_delay;
    world->tick_list = array_list_push(world->tick_list, &tick);
}

uint8_t world_is_solid(world_t *world, float x, float y, float z) {
    uint8_t block_id = world_get_block(world, x, y, z);
    return block_id != blocks.air.id && block_list[block_id].is_solid;
}

uint8_t world_is_solid_search(world_t *world, float x, float y, float z, float radius) {
    if (world_is_solid(world, x - radius, y - radius, z - radius)) return 1;
    if (world_is_solid(world, x + radius, y - radius, z - radius)) return 1;
    if (world_is_solid(world, x - radius, y + radius, z - radius)) return 1;
    if (world_is_solid(world, x + radius, y + radius, z - radius)) return 1;
    if (world_is_solid(world, x - radius, y - radius, z + radius)) return 1;
    if (world_is_solid(world, x + radius, y - radius, z + radius)) return 1;
    if (world_is_solid(world, x - radius, y + radius, z + radius)) return 1;
    if (world_is_solid(world, x + radius, y + radius, z + radius)) return 1;
    return 0;
}

int world_get_highest_block(world_t *world, int x, int z) {
    int y;
    for(y = world->height; (world_get_block(world, x, y - 1, z) == 0 || block_list[world_get_block(world, x, y - 1, z)].liquid_type != LIQUID_NONE) && y > 0; y--);
    return y;
}

void world_set_spawn_position(world_t *world, int x, int y, int z, float rotation) {
    world->spawn_x = x;
    world->spawn_y = y;
    world->spawn_z = z;
    world->spawn_rot = rotation;
}

float world_get_brightness(world_t *world, int x, int y, int z) {
    return world_is_lit(world, x, y, z) ? 1.0 : 0.6;
}

uint8_t world_is_water(world_t *world, int x, int y, int z) {
    uint8_t block_id = world_get_block(world, x, y, z);
    return block_id != blocks.air.id && block_list[block_id].liquid_type == LIQUID_WATER;
}

hit_result_t world_clip(world_t *world, vec3_t v0, vec3_t v1) {
    if(v0.x == NAN || v0.y == NAN || v0.z == NAN) return (hit_result_t){ .null = 1 };
    if(v1.x == NAN || v1.y == NAN || v1.z == NAN) return (hit_result_t){ .null = 1 };
    int i0x = v0.x, i0y = v0.y, i0z = v0.z;
    int i1x = v1.x, i1y = v1.y, i1z = v1.z;
    
    int i = 20;
    while(i-- >= 0) {
        if(v0.x == NAN || v0.y == NAN || v0.z == NAN) return (hit_result_t){ .null = 1 };
        if(i0x == i1x && i0y == i1y && i0z == i1z) return (hit_result_t){ .null = 1 };
        vec3_t a = { 999.0, 999.0, 999.0 };
        if(i1x > i0x) a.x = i0x + 1.0;
        if(i1x < i0x) a.x = i0x;
        if(i1y > i0y) a.y = i0y + 1.0;
        if(i1y < i0y) a.y = i0y;
        if(i1z > i0z) a.z = i0z + 1.0;
        if(i1z < i0z) a.z = i0z;
        vec3_t b = { 999.0, 999.0, 999.0 };
        vec3_t d = vec3_subtract(v1, v0);
        if(a.x != 999.0) b.x = (a.x - v0.x) / d.x;
        if(a.y != 999.0) b.y = (a.y - v0.y) / d.y;
        if(a.z != 999.0) b.z = (a.z - v0.z) / d.z;
        int c;
        if(b.x < b.y && b.x < b.z) {
            c = i1x > i0x ? 4 : 5;
            v0.x = a.x;
            v0.y += d.y * b.x;
            v0.z += d.z * b.x;
        }else if(b.y < b.z) {
            c = i1y > i0y ? 0 : 1;
            v0.y = a.y;
            v0.x += d.x * b.y;
            v0.z += d.z * b.y;
        }else {
            c = i1z > i0z ? 2 : 3;
            v0.z = a.z;
            v0.x += d.x * b.z;
            v0.y += d.y * b.z;
        }
        
        vec3_t v00 = { (int)v0.x, (int)v0.y, (int)v0.z };
        i0x = v00.x;
        i0y = v00.y;
        i0z = v00.z;
        if(c == 5) {
            i0x--;
            v00.x++;
        }
        if(c == 1) {
            i0y--;
            v00.y++;
        }
        if(c == 3) {
            i0z--;
            v00.z++;
        }
        uint8_t block_id = world_get_block(world, i0x, i0y, i0z);
        block_t *block = &block_list[block_id];
        if (block_id != blocks.air.id && block->liquid_type == LIQUID_NONE) {
            hit_result_t pos = block_clip(block, i0x, i0y, i0z, v0, v1);
            if (!pos.null) return pos;
        }
    }
    return (hit_result_t){ .null = 1 };
}

void world_play_sound(world_t *world, uint8_t sound, entity_t *entity, float volume, float pitch) {
    world_renderer_t *renderer = (world_renderer_t *)world->renderer;
    game_settings_t *settings = renderer->textures->settings;
    if(!settings->sound) return;
    if(vec3_distance_to_sqr((vec3_t){ entity->x, entity->y, entity->z }, (vec3_t){ renderer->world->player->x, renderer->world->player->y, renderer->world->player->z }) < 1024.0) {
        sounds_play_sound(&settings->minecraft->sounds, sound, volume, pitch);
    }
}

void world_play_sound_at(world_t *world, uint8_t sound, float x, float y, float z, float volume, float pitch) {
    world_renderer_t *renderer = (world_renderer_t *)world->renderer;
    game_settings_t *settings = renderer->textures->settings;
    if(!settings->sound) return;
    sounds_play_sound(&settings->minecraft->sounds, sound, volume, pitch);
}

uint8_t world_maybe_grow_tree(world_t *world, int x, int y, int z) {
    int r = (int)random_next_int_range(&world->random, 0, 2) + 4;
    uint8_t grow = 1;
    int i, j, k;
    for(j = y; j <= y + 1 + r; j++) {
        int offset = 1;
        if(j == y) offset = 0;
        if(j >= y + 1 + r - 2) offset = 2;

        for(i = x - offset; i <= x + offset && grow; i++) {
            for(k = z - offset; k <= z + offset && grow; k++) {
                if(i >= 0 && j >= 0 && k >= 0 && i < world->width && j < world->depth && k < world->height) {
                    if((world->blocks[(j * world->height + k) * world->width + i] & 0xFF) != 0) grow = 0;
                }else grow = 0;
            }
        }
    }

    if(!grow) return 0;
    if((world->blocks[((y - 1) * world->height + z) * world->width + x] & 0xFF) == blocks.grass.id && y < world->depth - r - 1) {
        world_set_block(world, x, y - 1, z, blocks.dirt.id);
        int l;
        for(l = y - 3 + r; l <= y + r; l++) {
            i = l - (y + r);
            k = 1 - i / 2;
            for(int m = x - k; m <= x + k; m++) {
                int diff = m - x;
                for(j = z - k; j <= z + k; j++) {
                    int diff2 = j - z;
                    if(abs(diff) != k || abs(diff2) != k || (random_next_int_range(&world->random, 0, 1) != 0 && i != 0)) {
                        world_set_block(world, m, l, j, blocks.leaves.id);
                    }
                }
            }
        }
        for(l = 0; l < r; l++) {
            world_set_block(world, x, y + l, z, blocks.log.id);
        }
        return 1;
    }
    return 0;
}

void world_explode(world_t *world, entity_t *entity, float x, float y, float z, float radius) {
    int x0 = (int)(x - radius - 1);
    int x1 = (int)(x + radius + 1);
    int y0 = (int)(y - radius - 1);
    int y1 = (int)(y + radius + 1);
    int z0 = (int)(z - radius - 1);
    int z1 = (int)(z + radius + 1);

    for(int i = x0; i < x1; i++) {
        for(int j = y1 - 1; j >= y0; j--) {
            for(int k = z0; k < z1; k++) {
                float x2 = i + 0.5 - x;
                float y2 = j + 0.5 - y;
                float z2 = k + 0.5 - z;
                uint8_t block_id = world_get_block(world, i, j, k);
                block_t *block = &block_list[block_id];
                if(i >= 0 && j >= 0 && k >= 0 && i < world->width && j < world->depth && k < world->height
                  && x2 * x2 + y2 * y2 + z2 * z2 < radius * radius && block_id > 0 && block->explodable) {
                    // block->spawn_items
                    world_set_block(world, i, j, k, blocks.air.id);
                    // block->was exploded
                }
            }
        }
    }

    entity_t ***entities = entity_map_get_entities(&world->entity_map, entity, x0, y0, z0, x1, y1, z1);
    printf("size: %d\n", array_list_length(*entities));

    for(int i = 0; i < array_list_length(*entities); i++) {
        entity_t *victim = array_list_get(*entities, i);
        float dist = vec3_distance_to((vec3_t){ x, y, z }, (vec3_t){ victim->x, victim->y, victim->z });
        printf("Explosion damage: %f %f %f\n", (1 - dist / radius) * 15 + 1, dist, radius);
        printf("creeper: %f %f %f\n", entity->x, entity->y, entity->z);
        printf("victim: %f %f %f\n", victim->x, victim->y, victim->z);
        if(dist / radius <= 1) {
            victim->hurt(victim, entity, (1 - dist / radius) * 15 + 1);
        }
    }
}

void world_add_entity(world_t *world, entity_t *entity) {
    entity_map_insert(&world->entity_map, entity);
    entity->world = (struct world_s *)world;
}

void world_remove_all_non_creative_entities(world_t *world) {
    entity_map_clear_non_creative_mode_entities(&world->entity_map);
}

void world_destroy(world_t *world) {
    array_list_free(world->entities);
    array_list_free(world->tick_list);
    free(world->light_depths);
    world->light_depths = NULL;
    free(world->blocks);
    world->blocks = NULL;
    entity_map_destroy(&world->entity_map);
}