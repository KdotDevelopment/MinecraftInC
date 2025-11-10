#pragma once

#include <world/block/block_sound.h>
#include <util/random.h>
#include <physics/AABB.h>
#include <particle/particles.h>
#include <world/material/materials.h>

#include <util/math_helper.h>

#include <stdint.h>

struct world_s;
struct entity_s;
struct tile_entity_s;

enum {
    LIQUID_NONE,
    LIQUID_WATER,
    LIQUID_LAVA,
};

enum {
    BLOCK_RENDER_TYPE_NORMAL,
    BLOCK_RENDER_TYPE_FLOWER,
    BLOCK_RENDER_TYPE_TORCH,
    BLOCK_RENDER_TYPE_FIRE,
    // Unsure what 4 is, it is in the block rendering code however
    BLOCK_RENDER_TYPE_GEARS = 5,
    BLOCK_RENDER_TYPE_CROPS,
};

typedef struct block_s {
    int texture_id;
    uint8_t id;
    block_sound_t *sound;
    float x0, y0, z0;
    float x1, y1, z1;
    float resistance;
    float hardness;
    float particle_gravity;
    material_t *material;
    uint8_t light_opacity;
    uint8_t light_value;
    uint8_t tick_rate;
    int render_type;
    uint8_t fire_spread_chance;
    uint8_t fire_catch_chance;

    uint8_t is_opaque;
    uint8_t is_solid;
    uint8_t is_cube;
    uint8_t tick_delay;
    uint8_t render_pass;
    uint8_t has_physics;
    uint8_t should_tick;
    uint8_t render_normal_block;
    uint8_t is_collidable;
    uint8_t is_container;

    union {
        struct {
            int liquid_still_id;
            int liquid_moving_id;
        };
        struct { // block_transparent
            uint8_t transparent_render_sides;
        };
        struct { // block_spawner (water and lava spawners)
            uint8_t spawner_liquid;
        };
        struct {
            uint8_t furnace_active;
        };
    };

    uint8_t (*can_render_side)(struct block_s *block, struct world_s *world, int x, int y, int z, uint8_t side);
    int (*get_texture_side)(struct block_s *block, uint8_t face);
    int (*get_texture_side_metadata)(struct block_s *block, uint8_t face, uint8_t metadata);
    int (*get_texture)(struct block_s *block, struct world_s *world, int x, int y, int z, uint8_t face);
    void (*update)(struct block_s *block, struct world_s *world, int x, int y, int z, random_t *random);
    void (*on_neighbor_changed)(struct block_s *block, struct world_s *world, int x, int y, int z, uint8_t block_id);
    void (*on_placed)(struct block_s *block, struct world_s *world, int x, int y, int z, uint8_t side);
    void (*on_added)(struct block_s *block, struct world_s *world, int x, int y, int z);
    void (*on_removed)(struct block_s *block, struct world_s *world, int x, int y, int z);
    void (*on_destroyed)(struct block_s *block, struct world_s *world, int x, int y, int z, uint8_t metadata);
    float (*get_brightness)(struct block_s *block, struct world_s *world, int x, int y, int z);
    void (*render_inside)(struct block_s *block, int x, int y, int z, uint8_t side);
    void (*render_preview)(struct block_s *block);
    uint8_t (*render)(struct block_s *block, struct world_s *world, int x, int y, int z);
    void (*render_full_brightness)(struct block_s *block);
    AABB_t (*get_selection_aabb)(struct block_s *block, int x, int y, int z);
    AABB_t (*get_collision_aabb)(struct block_s *block, int x, int y, int z);
    uint8_t (*get_drop_count)(struct block_s *block, random_t *random);
    void (*on_exploded)(struct block_s *block, struct world_s *world, int x, int y, int z);
    void (*visual_update)(struct block_s *block, struct world_s *world, int x, int y, int z, random_t *random);
    uint8_t (*can_place_at)(struct block_s *block, struct world_s *world, int x, int y, int z);
    int16_t (*get_item_dropped)(struct block_s *block, uint8_t metadata, random_t *random);
    int (*get_tick_rate)(struct block_s *block);
    struct tile_entity_s *(*get_tile_entity)(struct world_s *world);
    uint8_t (*on_interacted)(struct block_s *block, struct world_s *world, int x, int y, int z, struct entity_s *player);
    uint8_t (*can_grow_on)(struct block_s *block, uint8_t block_id);
    uint8_t (*can_stay)(struct block_s *block, struct world_s *world, int x, int y, int z);
    void (*on_walked_upon)(struct block_s *block, struct world_s *world, int x, int y, int z);
    hit_result_t (*clip)(struct block_s *block, struct world_s *world, int x, int y, int z, vec3_t v0, vec3_t v1);
} block_t;

extern block_t block_list[256];

block_t block_create(uint8_t id, int texture_id, block_sound_t *sound, float hardness, float resistance, material_t *material);
void block_set_bounds(block_t *block, float x0, float y0, float z0, float x1, float y1, float z1);
uint8_t block_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side);
int block_get_texture_side(block_t *block, uint8_t face);
int block_get_texture_side_metadata(block_t *block, uint8_t face, uint8_t metadata);
int block_get_texture(block_t *block, struct world_s *world, int x, int y, int z, uint8_t face);
void block_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);
void block_on_neighbor_changed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t block_id);
void block_on_placed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side);
void block_on_added(block_t *block, struct world_s *world, int x, int y, int z);
void block_on_removed(block_t *block, struct world_s *world, int x, int y, int z);
void block_breaking(block_t *block, struct world_s *world, int x, int y, int z, uint8_t face, particles_t *particles);
void block_on_destroyed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t metadata);
float block_get_brightness(block_t *block, struct world_s *world, int x, int y, int z);
void block_render_inside(block_t *block, int x, int y, int z, uint8_t side);
void block_render_side_with_texture(block_t *block, int x, int y, int z, uint8_t side, int tex);
void block_render_side(block_t *block, int x, int y, int z, uint8_t side);
void block_visual_update(block_t *block, struct world_s *world,int x, int y, int z, random_t *random);
void block_render_preview(block_t *block);
uint8_t block_render(block_t *block, struct world_s *world, int x, int y, int z);
void block_render_full_brightness(block_t *block);
hit_result_t block_clip(block_t *block, struct world_s *world, int x, int y, int z, vec3_t v0, vec3_t v1);
AABB_t block_get_selection_aabb(block_t *block, int x, int y, int z);
AABB_t block_get_collision_aabb(block_t *block, int x, int y, int z);
int16_t block_get_item_dropped(block_t *block, uint8_t metadata, random_t *random);
void block_spawn_items(block_t *block, struct world_s *world, int x, int y, int z, uint8_t metadata);
void block_spawn_items_chance(block_t *block, struct world_s *world, int x, int y, int z, uint8_t metadata, float chance);
float block_get_strength(block_t *block, entity_t *player);
uint8_t block_get_drop_count(block_t *block, random_t *random);
uint8_t block_can_place_at(block_t *block, struct world_s *world, int x, int y, int z);
void block_on_exploded(block_t *block, struct world_s *world, int x, int y, int z);
int block_get_tick_rate(block_t *block);
struct tile_entity_s *block_get_tile_entity(struct world_s *world);
uint8_t block_on_interacted(block_t *block, struct world_s *world, int x, int y, int z, struct entity_s *player);
uint8_t block_can_grow_on(block_t *block, uint8_t block_id);
uint8_t block_can_stay(block_t *block, struct world_s *world, int x, int y, int z);
void block_on_walked_upon(block_t *block, struct world_s *world, int x, int y, int z);