#pragma once

#include <util/random.h>
#include <level/next_tick_data.h>
#include <level/levelgen/level_gen.h>
#include <player/player.h>
#include <player/hit_result.h>
#include <gui/progress_bar.h>
#include <particle/particles.h>
#include <level/entity_map.h>

#include <stdint.h>

struct enitity_s;
struct minecraft_s;

typedef struct level_s {
    int width;
    int height;
    int depth;
    uint8_t *blocks;
    int32_t spawn_x;
    int32_t spawn_y;
    int32_t spawn_z;
    float spawn_rot;
    random_t random;
    int random_value;
    next_tick_data_t *tick_list; //arraylist
    int32_t water_level;
    uint32_t sky_color;
    uint32_t fog_color;
    uint32_t cloud_color;
    struct level_renderer_s *renderer;
    level_gen_t generator;
    entity_t **entities; //arraylist
    progress_bar_t *progress_bar;
    particles_t *particles;
    entity_map_t entity_map;
    int unprocessed;
    int tick_count;
    uint8_t creative_mode;
    int *light_depths;
    player_t *player;
    struct minecraft_s *minecraft;
} level_t;

void level_create(level_t *level, struct minecraft_s *minecraft, progress_bar_t *progress_bar, int size);
void level_regenerate(level_t *level, int size);
void level_set_data(level_t *level, int w, int d, int h, uint8_t *blocks);
void level_find_spawn(level_t *level);
void level_calculate_light_depths(level_t *level, int x0, int y0, int x1, int y1);
uint8_t level_is_light_blocker(level_t *level, int x, int y, int z);
AABB_t *level_get_cubes(level_t *level, AABB_t box);
void level_swap(level_t *level, int x0, int y0, int z0, int x1, int y1, int z1);
uint8_t level_set_block(level_t *level, int x, int y, int z, uint8_t block_id);
uint8_t level_set_block_no_neighbor_change(level_t *level, int x, int y, int z, uint8_t block_id);
void level_update_block(level_t *level, int x, int y, int z, uint8_t block_id);
void level_update_neighbors_at(level_t *level, int x, int y, int z, uint8_t block_id);
uint8_t level_set_block_no_update(level_t *level, int x, int y, int z, uint8_t block_id);
uint8_t level_is_lit(level_t *level, int x, int y, int z);
uint8_t level_get_block(level_t *level, int x, int y, int z);
uint8_t level_is_solid_block(level_t *level, int x, int y, int z);
void level_tick_entities(level_t *level);
uint8_t level_is_free(level_t *level, AABB_t bb);
void level_tick(level_t *level);
uint8_t level_is_in_bounds(level_t *level, int x, int y, int z);
float level_get_ground_level(level_t *level);
float level_get_water_level(level_t *level);
uint8_t level_contains_any_liquid(level_t *level, AABB_t box);
uint8_t level_contains_liquid(level_t *level, AABB_t box, uint8_t liquid_type);
void level_add_to_next_tick(level_t *level, int x, int y, int z, uint8_t block_id);
uint8_t level_is_solid(level_t *level, float x, float y, float z);
uint8_t level_is_solid_search(level_t *level, float x, float y, float z, float radius);
int level_get_highest_block(level_t *level, int x, int z);
void level_set_spawn_position(level_t *level, int x, int y, int z, float rotation);
float level_get_brightness(level_t *level, int x, int y, int z);
uint8_t level_is_water(level_t *level, int x, int y, int z);
hit_result_t level_clip(level_t *level, vec3_t v0, vec3_t v1);
void level_play_sound(level_t *level, char *sound, entity_t *entity, float volume, float pitch);
void level_play_sound_at(level_t *level, char *sound, float x, float y, float z, float volume, float pitch);
uint8_t level_maybe_grow_tree(level_t *level, int x, int y, int z);
void level_add_entity(level_t *level, entity_t *entity);
void level_remove_all_non_creative_entities(level_t *level);
void level_explode(level_t *level, entity_t *entity, float x, float y, float z, float radius);
void level_destroy(level_t *level);