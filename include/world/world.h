#pragma once

#include <util/random.h>
#include <world/next_tick_data.h>
#include <world/terrain/chunk_provider_generate.h>
#include <player/player.h>
#include <player/hit_result.h>
#include <gui/progress_bar.h>
#include <particle/particles.h>
#include <nbt/nbt_tag_compound.h>
#include <world/block/tile_entity/tile_entity.h>
#include <world/chunk/chunk_provider.h>
#include <renderer/renderer_world.h>

#include <stdint.h>
#include <stdio.h>

#define HASH_MAGIC 0x3C6EF35F
#define WORLD_MAX_SIZE 32000000

struct enitity_s;
struct minecraft_s;
struct chunk_s;

enum {
    LIGHT_TYPE_SKY = 15,
    LIGHT_TYPE_BLOCK = 0
};

typedef struct chunk_metadata_s {
    uint8_t light_type;
    int x;
    int y;
    int z;
    int max_x;
    int max_y;
    int max_z;
} chunk_metadata_t;

typedef struct world_s {
    uint64_t *lighting_update_list; //chunk_metadata_t
    uint64_t *loaded_entity_list; // entity_t *
    next_tick_data_t *next_tick_data_list; // next_tick_data_t
    uint64_t *loaded_tile_entity_list; // tile_entity_t *
    uint64_t *renderer_world_list; // renderer_world_t *
    int32_t spawn_x;
    int32_t spawn_y;
    int32_t spawn_z;
    int64_t world_time;
    uint8_t difficulty_setting;
    //pathfinder_t pathfinder;
    random_t random;
    uint32_t sky_color;
    uint32_t fog_color;
    uint32_t cloud_color;
    uint8_t skylight_subtracted; // time of day based
    float light_brightness_table[16];
    uint8_t is_new_world;
    int64_t random_seed;
    char *save_file; // includes world name
    chunk_provider_t chunk_provider;
    chunk_provider_t chunk_provider_gen;
    nbt_base_t player_nbt;
    int64_t size_on_disk;

    uint8_t creative_mode; // remove later

    int64_t random_number;
    particles_t *particles;
    player_t *player;
    struct minecraft_s *minecraft;
} world_t;

void world_create(world_t *world, struct minecraft_s *minecraft, char *saves_dir, char *world_name, int64_t seed);
void world_spawn_player(world_t *world);
void world_save(world_t *world, uint8_t check_entities);
uint8_t world_get_block(world_t *world, int x, int y, int z);
uint8_t world_block_exists(world_t *world, int x, int y, int z);
uint8_t world_chunk_exists(world_t *world, int x, int z);
struct chunk_s *world_get_chunk(world_t *world, int x, int z);
uint8_t world_set_block_no_update(world_t *world, int x, int y, int z, uint8_t block_id);
material_t *world_get_block_material(world_t *world, int x, int y, int z);
uint8_t world_get_block_metadata(world_t *world, int x, int y, int z);
uint8_t world_set_block_metadata(world_t *world, int x, int y, int z, uint8_t data);
uint8_t world_set_block_with_update(world_t *world, int x, int y, int z, uint8_t block_id);
void world_mark_blocks_dirty_vertical(world_t *world, int x, int z, int y1, int y2);
void world_swap(world_t *world, int x0, int y0, int z0, int x1, int y1, int z1);
void world_update_neighbors_at(world_t *world, int x, int y, int z, uint8_t block_id);
void world_update_block(world_t *world, int x, int y, int z, uint8_t block_id);
uint8_t world_can_block_see_sky(world_t *world, int x, int y, int z);
uint8_t world_get_block_light_value(world_t *world, int x, int y, int z);
uint8_t world_can_existing_block_see_sky(world_t *world, int x, int y, int z);
int world_get_height_value(world_t *world, int x, int z);
void world_light_changed(world_t *world, uint8_t light_type, int x, int y, int z, int value);
int world_get_saved_light_value(world_t *world, uint8_t light_type, int x, int y, int z);
float world_get_brightness(world_t *world, int x, int y, int z);
hit_result_t world_clip(world_t *world, vec3_t v0, vec3_t v1);
void world_play_sound_at_entity(world_t *world, entity_t *entity, uint8_t sound, float volume, float pitch);
void world_play_sound(world_t *world, double x, double y, double z, uint8_t sound, float volume, float pitch);
void world_spawn_particle(world_t *world, uint8_t particle_type, double x, double y, double z, double x_vel, double y_vel, double z_vel);
void world_spawn_entity(world_t *world, entity_t *entity);
void world_set_entity_dead(world_t *world, entity_t *entity);
void world_add_renderer(world_t *world, renderer_world_t *renderer);
void world_remove_renderer(world_t *world, renderer_world_t *renderer);
AABB_t *world_get_cubes(world_t *world, AABB_t box);
vec3_t world_get_sky_color(world_t *world, float time);
float world_get_celestial_angle(world_t *world, float partial_tick);
vec3_t world_get_cloud_color(world_t *world, float partial_tick);
vec3_t world_get_fog_color(world_t *world, float partial_tick);
float world_get_star_brightness(world_t *world, float partial_tick);
void world_schedule_block_update(world_t *world, int x, int y, int z, uint8_t block_id);
void world_update_entities(world_t *world);
uint8_t world_is_aabb_clear(world_t *world, AABB_t box);
uint8_t world_is_liquid_in_range(world_t *world, AABB_t box);
uint8_t world_is_bounding_box_burning(world_t *world, AABB_t box);
uint8_t world_is_material_in_box(world_t *world, AABB_t box, material_t *material);
void world_explode(world_t *world, entity_t *source_entity, double x, double y, double z, float radius);
float world_get_block_density(world_t *world, vec3_t pos, AABB_t box);
void world_extinguish_fire(world_t *world, int x, int y, int z, uint8_t side);
// world_debug_loaded_entities
tile_entity_t *world_get_tile_entity(world_t *world, int x, int y, int z);
void world_set_tile_entity(world_t *world, int x, int y, int z, tile_entity_t *tile_entity);
void world_remove_tile_entity(world_t *world, int x, int y, int z);
uint8_t world_is_solid(world_t *world, int x, int y, int z);
// world_save_indirectly
uint8_t world_update_lighting(world_t *world);
void world_schedule_light_update(world_t *world, uint8_t light_type, int x0, int y0, int z0, int x1, int y1, int z1);
void world_restart_time_of_day(world_t *world);
void world_visual_update(world_t *world, int x, int y, int z);
entity_t **world_get_entities_excluding(world_t *world, entity_t *entity, AABB_t box);

void world_set_spawn_position(world_t *world, int x, int y, int z);
uint8_t world_is_water(world_t *world, int x, int y, int z);
void world_destroy(world_t *world);