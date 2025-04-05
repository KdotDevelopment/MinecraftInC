#include <level/level.h>
#include <renderer/level_renderer.h>
#include <level/block/block.h>
#include <level/block/blocks.h>
#include <sound/sounds.h>
#include <minecraft.h>

#include <util/array_list.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void level_create(level_t *level, struct minecraft_s *minecraft, progress_bar_t *progress_bar, int size) {
    //level_t level = { 0 };
    
    level->random = random_create(time(NULL));
    level->minecraft = minecraft;
    level->sky_color = 0x99CCFFFF;
    level->fog_color = 0xFFFFFFFF;
    level->cloud_color = 0xFFFFFFFF;
    level->unprocessed = 0;
    level->tick_count = 0;
    level->tick_list = array_list_create(sizeof(next_tick_data_t));
    level->random = random_create(time(NULL));
    level->random_value = (int)random_next_int(&level->random);
    level->generator = level_gen_create();
    level->entities = array_list_create(sizeof(entity_t *));
    level->progress_bar = progress_bar;
    int block_count = 128 << size;
    level_gen_generate(&level->generator, block_count, block_count, (struct level_s *)level);

    //return level;
}

void level_regenerate(level_t *level, int size) {
    level_destroy(level);
    level_create(level, level->minecraft, level->progress_bar, size);
}

void level_set_data(level_t *level, int w, int d, int h, uint8_t *blocks) {
    if(level->blocks) free(level->blocks);
    level->blocks = NULL;
    if(level->light_depths) free(level->light_depths);
    level->light_depths = NULL;
    level->width = w;
    level->height = h;
    level->depth = d;
    entity_map_create(&level->entity_map, level->width, level->depth, level->height);
    level->blocks = calloc(w * d * h, 1);
    memcpy(level->blocks, blocks, w * d * h);
    level->light_depths = calloc(w * h, sizeof(int));
    for(int i = 0; i < w * h; i++) {
        level->light_depths[i] = level->depth - 1;
    }
    level_calculate_light_depths(level, 0, 0, w, h);
    for (int i = 0; i < array_list_length(level->tick_list); i++) {
        next_tick_data_t *tick = &level->tick_list[i];
        free(tick);
    }
    level->tick_list = array_list_clear(level->tick_list);
    level_find_spawn(level);
}

void level_find_spawn(level_t *level) {
    int i = 0;
    int x;
    int y;
    int z;
    do {
        i++;
        x = rand() % ((level->width / 2) - (level->width / 4)) + (level->width / 4);
        z = rand() % ((level->height / 2) - (level->height / 4)) + (level->height / 4);
        y = level_get_highest_block(level, x, z) + 1;
        if(i == 10000) {
            level->spawn_x = x;
            level->spawn_y = -100;
            level->spawn_z = z;
            return;
        }
    }while(y <= level_get_water_level(level));
    level->spawn_x = x;
    level->spawn_y = y;
    level->spawn_z = z;
}

void level_calculate_light_depths(level_t *level, int x0, int y0, int x1, int y1) {
    for(int i = x0; i < x0 + x1; i++) {
        for(int j = y0; j < y0 + y1; j++) {
            int depth = level->light_depths[i + j * level->width];
            int k = level->depth - 1;
            while(!level_is_light_blocker(level, i, k, j) && k > 0) k--;
            level->light_depths[i + j * level->width] = k;
            if(depth != k) {
                int min = depth < k ? depth : k;
				int max = depth > k ? depth : k;
                if(level->renderer != NULL) level_renderer_queue_chunks((level_renderer_t *)level->renderer, i - 1, min - 1, j - 1, i + 1, max + 1, j + 1);
            }
        }
    }
}

uint8_t level_is_light_blocker(level_t *level, int x, int y, int z) {
    block_t *block = &block_list[level_get_block(level, x, y, z)];
    return block->id == blocks.air.id ? 0 : block->is_opaque;
}

AABB_t *level_get_cubes(level_t *level, AABB_t box) {
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
                if(i >= 0 && j >= 0 && k >= 0 && i < level->width && j < level->depth && k < level->height) {
					block_t *block = &block_list[level_get_block(level, i, j, k)];
					if (block->id != blocks.air.id) {
						bb = block->get_collision_aabb(block, i, j, k);
						if(!bb.null && AABB_intersects_inner(box, bb)) {
                            list = array_list_push(list, &bb);
                        }
					}
				}else if(i < 0 || j < 0 || k < 0 || i >= level->width || k >= level->height) {
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

void level_swap(level_t *level, int x0, int y0, int z0, int x1, int y1, int z1) {
    uint8_t t1 = level_get_block(level, x0, y0, z0);
    uint8_t t2 = level_get_block(level, x1, y1, z1);
    level_set_block_no_neighbor_change(level, x0, y0, z0, t2);
    level_set_block_no_neighbor_change(level, x1, y1, z1, t1);
    level_update_neighbors_at(level, x0, y0, z0, t2);
    level_update_neighbors_at(level, x1, y1, z1, t1);
}

uint8_t level_set_block(level_t *level, int x, int y, int z, uint8_t block_id) {
    if(level_set_block_no_neighbor_change(level, x, y, z, block_id)) {
        level_update_neighbors_at(level, x, y, z, block_id);
        return 1;
    }
    return 0;
}

uint8_t level_set_block_no_neighbor_change(level_t *level, int x, int y, int z, uint8_t block_id) {
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height) return 0;
    int i = (y * level->height + z) * level->width + x;
    if(block_id == level->blocks[i]) return 0;

    if(block_id == blocks.air.id && (x == 0 || z == 0 || x == level->width - 1 || z == level->height - 1) && y >= level_get_ground_level(level) && y < level_get_water_level(level)) {
        block_id = blocks.water.id;
    }
    uint8_t prev = level->blocks[i];
    level->blocks[i] = block_id;
    block_t *block = &block_list[block_id];
    block_t *prev_block = &block_list[prev];
    if(prev != blocks.air.id) {
        prev_block->on_removed(prev_block, (struct level_s *)level, x, y, z);
    }
    if(block_id != blocks.air.id) {
        block->on_added(block, (struct level_s *)level, x, y, z);
    }
    level_calculate_light_depths(level, x, z, 1, 1);
    if(level->renderer != NULL) {
        level_renderer_queue_chunks((level_renderer_t *)level->renderer, x - 1, y - 1, z - 1, x + 1, y + 1, z + 1);
    }

    return 1;
}

void level_update_block(level_t *level, int x, int y, int z, uint8_t block_id) {
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height) return;
	block_t *block = &block_list[level->blocks[(y * level->height + z) * level->width + x]];
	if (block->id != blocks.air.id) { 
        block->on_neighbor_changed(block, (struct level_s *)level, x, y, z, block_id);
    }
}

void level_update_neighbors_at(level_t *level, int x, int y, int z, uint8_t block_id) {
    level_update_block(level, x - 1, y, z, block_id);
    level_update_block(level, x + 1, y, z, block_id);
    level_update_block(level, x, y - 1, z, block_id);
    level_update_block(level, x, y + 1, z, block_id);
    level_update_block(level, x, y, z - 1, block_id);
    level_update_block(level, x, y, z + 1, block_id);
}

uint8_t level_set_block_no_update(level_t *level, int x, int y, int z, uint8_t block_id) {
    if (x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height) return 0;
	int i = (y * level->height + z) * level->width + x;
	if (block_id == level->blocks[i]) return 0;
	level->blocks[i] = block_id;
	return 1;
}

uint8_t level_is_lit(level_t *level, int x, int y, int z) {
    return !(x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height) ? y >= level->light_depths[x + z * level->width] : 1;
}

uint8_t level_get_block(level_t *level, int x, int y, int z) {
    if(x < 0 || y < 0 || z < 0
        || x >= level->width
        || y >= level->depth
        || z >= level->height) {
        return 0;
    }

    return level->blocks[(y * level->height + z) * level->width + x];
}

uint8_t level_is_solid_block(level_t *level, int x, int y, int z) {
    block_t *block = &block_list[level_get_block(level, x, y, z)];
    return block->is_solid;
}

void level_tick_entities(level_t *level) {
    entity_map_tick_all(&level->entity_map);
}

uint8_t level_is_free(level_t *level, AABB_t bb) {
    entity_t ***entities = entity_map_get_entities(&level->entity_map, NULL, bb.x0, bb.y0, bb.z0, bb.x1, bb.y1, bb.z1);
    return array_list_length(*entities) == 0;
}

void level_tick(level_t *level) {
    level->tick_count++;
    int a = 1;
    int b = 1;
    for(b = 1; 1 << a < level->width; a++);
    while(1 << b < level->height) b++;

    if(level->tick_count % 5 == 0) {
        for(int i = 0; i < array_list_length(level->tick_list); i++) {
            next_tick_data_t next_tick = level->tick_list[0];
            level->tick_list = array_list_remove(level->tick_list, 0);
            if(next_tick.ticks > 0) {
                next_tick.ticks--;
                level->tick_list = array_list_push(level->tick_list, &next_tick);
            }else {
                uint8_t block_id = level->blocks[(next_tick.y * level->height + next_tick.z) * level->width + next_tick.x];
                block_t *block = &block_list[block_id];
                if(level_is_in_bounds(level, next_tick.x, next_tick.y, next_tick.z) && block_id == next_tick.type && block_id != blocks.air.id) {
                    if(block->should_tick) {
                        block->update(block, (struct level_s *)level, next_tick.x, next_tick.y, next_tick.z, &level->random);
                    }
                }
            }
        }
    }

    level->unprocessed += level->width * level->height * level->depth;
    int c = level->unprocessed / 200;
    level->unprocessed -= c * 200;

    for(int i = 0; i < c; i++) {
        level->random_value = level->random_value * 3 + 1013904223;
        int y = level->random_value >> 2;
        int x = y & (level->width - 1);
        int z = y >> a & (level->height - 1);
        y = y >> (a + b) & (level->depth - 1);
        uint8_t block_id = level->blocks[(y * level->height + z) * level->width + x];
        block_t *block = &block_list[block_id];
        if(block->should_tick) {
            block->update(block, (struct level_s *)level, x, y, z, &level->random);
        }
    }
}

uint8_t level_is_in_bounds(level_t *level, int x, int y, int z) {
    return !(x < 0 || y < 0 || z < 0 || x >= level->width || y >= level->depth || z >= level->height);  
}

float level_get_ground_level(level_t *level) {
    return level_get_water_level(level) - 2;
}

float level_get_water_level(level_t *level) {
    return level->water_level;
}

uint8_t level_contains_any_liquid(level_t *level, AABB_t box) {
    int x0 = box.x0, y0 = box.y0, z0 = box.z0;
	int x1 = box.x1 + 1, y1 = box.y1 + 1, z1 = box.z1 + 1;
	if(box.x0 < 0.0) x0--;
	if(box.y0 < 0.0) y0--;
	if(box.z0 < 0.0) z0--;
	if(x0 < 0) x0 = 0;
	if(y0 < 0) y0 = 0;
	if(z0 < 0) z0 = 0;
	if(x1 > level->width) x1 = level->width;
	if(y1 > level->depth) y1 = level->depth;
	if(z1 > level->height) z1 = level->height;
	for(int i = x0; i < x1; i++) {
		for(int j = y0; j < y1; j++) {
			for(int k = z0; k < z1; k++) {
				block_t *block = &block_list[level_get_block(level, i, j, k)];
				if (block->id != blocks.air.id && block->liquid_type != LIQUID_NONE) return 1;
			}
		}
	}
	return 0;
}

uint8_t level_contains_liquid(level_t *level, AABB_t box, uint8_t liquid_type) {
    int x0 = box.x0, y0 = box.y0, z0 = box.z0;
	int x1 = box.x1 + 1, y1 = box.y1 + 1, z1 = box.z1 + 1;
	if(box.x0 < 0.0) x0--;
	if(box.y0 < 0.0) y0--;
	if(box.z0 < 0.0) z0--;
	if(x0 < 0) x0 = 0;
	if(y0 < 0) y0 = 0;
	if(z0 < 0) z0 = 0;
	if(x1 > level->width) x1 = level->width;
	if(y1 > level->depth) y1 = level->depth;
	if(z1 > level->height) z1 = level->height;
	for(int i = x0; i < x1; i++) {
		for(int j = y0; j < y1; j++) {
			for(int k = z0; k < z1; k++) {
				block_t *block = &block_list[level_get_block(level, i, j, k)];
				if (block->id != blocks.air.id && block->liquid_type == liquid_type) return 1;
			}
		}
	}
	return 0;
}

void level_add_to_next_tick(level_t *level, int x, int y, int z, uint8_t block_id) {
    next_tick_data_t tick = { .x = x, .y = y, .z = z, .type = block_id };
    block_t *block = &block_list[block_id];
    if(block_id != blocks.air.id) tick.ticks = block->tick_delay;
    level->tick_list = array_list_push(level->tick_list, &tick);
}

uint8_t level_is_solid(level_t *level, float x, float y, float z) {
    uint8_t block_id = level_get_block(level, x, y, z);
    return block_id != blocks.air.id && block_list[block_id].is_solid;
}

uint8_t level_is_solid_search(level_t *level, float x, float y, float z, float radius) {
    if (level_is_solid(level, x - radius, y - radius, z - radius)) return 1;
	if (level_is_solid(level, x + radius, y - radius, z - radius)) return 1;
	if (level_is_solid(level, x - radius, y + radius, z - radius)) return 1;
	if (level_is_solid(level, x + radius, y + radius, z - radius)) return 1;
	if (level_is_solid(level, x - radius, y - radius, z + radius)) return 1;
	if (level_is_solid(level, x + radius, y - radius, z + radius)) return 1;
	if (level_is_solid(level, x - radius, y + radius, z + radius)) return 1;
	if (level_is_solid(level, x + radius, y + radius, z + radius)) return 1;
	return 0;
}

int level_get_highest_block(level_t *level, int x, int z) {
    int y;
    for(y = level->height; (level_get_block(level, x, y - 1, z) == 0 || block_list[level_get_block(level, x, y - 1, z)].liquid_type != LIQUID_NONE) && y > 0; y--);
    return y;
}

void level_set_spawn_position(level_t *level, int x, int y, int z, float rotation) {
    level->spawn_x = x;
    level->spawn_y = y;
    level->spawn_z = z;
    level->spawn_rot = rotation;
}

float level_get_brightness(level_t *level, int x, int y, int z) {
    return level_is_lit(level, x, y, z) ? 1.0 : 0.6;
}

uint8_t level_is_water(level_t *level, int x, int y, int z) {
    uint8_t block_id = level_get_block(level, x, y, z);
    return block_id != blocks.air.id && block_list[block_id].liquid_type == LIQUID_WATER;
}

hit_result_t level_clip(level_t *level, vec3_t v0, vec3_t v1) {
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
		uint8_t block_id = level_get_block(level, i0x, i0y, i0z);
		block_t *block = &block_list[block_id];
		if (block_id != blocks.air.id && block->liquid_type == LIQUID_NONE) {
			hit_result_t pos = block_clip(block, i0x, i0y, i0z, v0, v1);
			if (!pos.null) return pos;
		}
	}
	return (hit_result_t){ .null = 1 };
}

void level_play_sound(level_t *level, char *sound, entity_t *entity, float volume, float pitch) {
    level_renderer_t *renderer = (level_renderer_t *)level->renderer;
    game_settings_t *settings = renderer->textures->settings;
    if(!settings->sound) return;
    if(vec3_distance_to_sqr((vec3_t){ entity->x, entity->y, entity->z }, (vec3_t){ renderer->level->player->x, renderer->level->player->y, renderer->level->player->z }) < 1024.0) {
        sounds_play_sound(&settings->minecraft->sounds, sound, volume, pitch);
    }
}

void level_play_sound_at(level_t *level, char *sound, float x, float y, float z, float volume, float pitch) {
    level_renderer_t *renderer = (level_renderer_t *)level->renderer;
    game_settings_t *settings = renderer->textures->settings;
    if(!settings->sound) return;
    sounds_play_sound(&settings->minecraft->sounds, sound, volume, pitch);
}

uint8_t level_maybe_grow_tree(level_t *level, int x, int y, int z) {
    int r = (int)random_next_int_range(&level->random, 0, 2) + 4;
    uint8_t grow = 1;
    int i, j, k;
    for(j = y; j <= y + 1 + r; j++) {
        int offset = 1;
        if(j == y) offset = 0;
        if(j >= y + 1 + r - 2) offset = 2;

        for(i = x - offset; i <= x + offset && grow; i++) {
            for(k = z - offset; k <= z + offset && grow; k++) {
                if(i >= 0 && j >= 0 && k >= 0 && i < level->width && j < level->depth && k < level->height) {
                    if((level->blocks[(j * level->height + k) * level->width + i] & 0xFF) != 0) grow = 0;
                }else grow = 0;
            }
        }
    }

    if(!grow) return 0;
    if((level->blocks[((y - 1) * level->height + z) * level->width + x] & 0xFF) == blocks.grass.id && y < level->depth - r - 1) {
        level_set_block(level, x, y - 1, z, blocks.dirt.id);
        int l;
        for(l = y - 3 + r; l <= y + r; l++) {
            i = l - (y + r);
            k = 1 - i / 2;
            for(int m = x - k; m <= x + k; m++) {
                int diff = m - x;
                for(j = z - k; j <= z + k; j++) {
                    int diff2 = j - z;
                    if(abs(diff) != k || abs(diff2) != k || (random_next_int_range(&level->random, 0, 1) != 0 && i != 0)) {
                        level_set_block(level, m, l, j, blocks.leaves.id);
                    }
                }
            }
        }
        for(l = 0; l < r; l++) {
            level_set_block(level, x, y + l, z, blocks.log.id);
        }
        return 1;
    }
    return 0;
}

void level_explode(level_t *level, entity_t *entity, float x, float y, float z, float radius) {
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
                uint8_t block_id = level_get_block(level, i, j, k);
                block_t *block = &block_list[block_id];
                if(i >= 0 && j >= 0 && k >= 0 && i < level->width && j < level->depth && k < level->height
                  && x2 * x2 + y2 * y2 + z2 * z2 < radius * radius && block_id > 0 && block->explodable) {
                    // block->spawn_items
                    level_set_block(level, i, j, k, blocks.air.id);
                    // block->was exploded
                }
            }
        }
    }

    entity_t ***entities = entity_map_get_entities(&level->entity_map, entity, x0, y0, z0, x1, y1, z1);
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

void level_add_entity(level_t *level, entity_t *entity) {
    entity_map_insert(&level->entity_map, entity);
    entity->level = (struct level_s *)level;
}

void level_remove_all_non_creative_entities(level_t *level) {
    entity_map_clear_non_creative_mode_entities(&level->entity_map);
}

void level_destroy(level_t *level) {
    array_list_free(level->entities);
    array_list_free(level->tick_list);
    free(level->light_depths);
    level->light_depths = NULL;
    free(level->blocks);
    level->blocks = NULL;
    entity_map_destroy(&level->entity_map);
}