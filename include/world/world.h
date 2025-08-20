#pragma once

#include <util/random.h>
#include <world/next_tick_data.h>
#include <world/terrain/chunk_provider_generator.h>
#include <player/player.h>
#include <player/hit_result.h>
#include <gui/progress_bar.h>
#include <particle/particles.h>
#include <world/entity_map.h>

#include <stdint.h>

struct enitity_s;
struct minecraft_s;

typedef struct world_s {
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
    int32_t water_world;
    uint32_t sky_color;
    uint32_t fog_color;
    uint32_t cloud_color;
    struct world_renderer_s *renderer;
    world_gen_t generator;
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
} world_t;

void world_create(world_t *world, struct minecraft_s *minecraft, progress_bar_t *progress_bar, int size);
void world_regenerate(world_t *world, int size);
void world_set_data(world_t *world, int w, int d, int h, uint8_t *blocks);
void world_find_spawn(world_t *world);
void world_calculate_light_depths(world_t *world, int x0, int y0, int x1, int y1);
uint8_t world_is_light_blocker(world_t *world, int x, int y, int z);
AABB_t *world_get_cubes(world_t *world, AABB_t box);
void world_swap(world_t *world, int x0, int y0, int z0, int x1, int y1, int z1);
uint8_t world_set_block(world_t *world, int x, int y, int z, uint8_t block_id);
uint8_t world_set_block_no_neighbor_change(world_t *world, int x, int y, int z, uint8_t block_id);
void world_update_block(world_t *world, int x, int y, int z, uint8_t block_id);
void world_update_neighbors_at(world_t *world, int x, int y, int z, uint8_t block_id);
uint8_t world_set_block_no_update(world_t *world, int x, int y, int z, uint8_t block_id);
uint8_t world_is_lit(world_t *world, int x, int y, int z);
uint8_t world_get_block(world_t *world, int x, int y, int z);
uint8_t world_is_solid_block(world_t *world, int x, int y, int z);
void world_tick_entities(world_t *world);
uint8_t world_is_free(world_t *world, AABB_t bb);
void world_tick(world_t *world);
uint8_t world_is_in_bounds(world_t *world, int x, int y, int z);
float world_get_ground_world(world_t *world);
float world_get_water_world(world_t *world);
uint8_t world_contains_any_liquid(world_t *world, AABB_t box);
uint8_t world_contains_liquid(world_t *world, AABB_t box, uint8_t liquid_type);
void world_add_to_next_tick(world_t *world, int x, int y, int z, uint8_t block_id);
uint8_t world_is_solid(world_t *world, float x, float y, float z);
uint8_t world_is_solid_search(world_t *world, float x, float y, float z, float radius);
int world_get_highest_block(world_t *world, int x, int z);
void world_set_spawn_position(world_t *world, int x, int y, int z, float rotation);
float world_get_brightness(world_t *world, int x, int y, int z);
uint8_t world_is_water(world_t *world, int x, int y, int z);
hit_result_t world_clip(world_t *world, vec3_t v0, vec3_t v1);
void world_play_sound(world_t *world, uint8_t sound, entity_t *entity, float volume, float pitch);
void world_play_sound_at(world_t *world, uint8_t sound, float x, float y, float z, float volume, float pitch);
uint8_t world_maybe_grow_tree(world_t *world, int x, int y, int z);
void world_add_entity(world_t *world, entity_t *entity);
void world_remove_all_non_creative_entities(world_t *world);
void world_explode(world_t *world, entity_t *entity, float x, float y, float z, float radius);
void world_destroy(world_t *world);