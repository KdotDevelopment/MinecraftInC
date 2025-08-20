#pragma once

#include <world/block/block_sound.h>
#include <util/random.h>
#include <physics/AABB.h>
#include <particle/particles.h>
#include <world/material/material.h>

#include <util/sin_table.h>

#include <stdint.h>

struct world_s;

enum {
    LIQUID_NONE,
    LIQUID_WATER,
    LIQUID_LAVA,
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
    uint8_t drop_id;

    uint8_t is_opaque;
    uint8_t is_solid;
    uint8_t is_cube;
    uint8_t tick_delay;
    uint8_t render_pass;
    uint8_t has_physics;
    uint8_t should_tick;

    uint8_t liquid_type;
    int liquid_still_id;
    int liquid_moving_id;

    uint8_t (*can_render_side)(struct block_s *block, struct world_s *world, int x, int y, int z, int side);
    int (*get_texture_id)(struct block_s *block, int face);
    void (*update)(struct block_s *block, struct world_s *world, int x, int y, int z, random_t *random);
    void (*on_neighbor_changed)(struct block_s *block, struct world_s *world, int x, int y, int z, uint8_t block_id);
    void (*on_placed)(struct block_s *block, struct world_s *world, int x, int y, int z);
    void (*on_added)(struct block_s *block, struct world_s *world, int x, int y, int z);
    void (*on_removed)(struct block_s *block, struct world_s *world, int x, int y, int z);
    void (*destroy)(struct block_s *block, struct world_s *world, int x, int y, int z, particles_t *particles);
    float (*get_brightness)(struct block_s *block, struct world_s *world, int x, int y, int z);
    void (*render_inside)(struct block_s *block, int x, int y, int z, int side);
    void (*render_preview)(struct block_s *block);
    uint8_t (*render)(struct block_s *block, struct world_s *world, int x, int y, int z);
    void (*render_full_brightness)(struct block_s *block);
    AABB_t (*get_selection_aabb)(struct block_s *block, int x, int y, int z);
    AABB_t (*get_collision_aabb)(struct block_s *block, int x, int y, int z);
    uint8_t (*get_drop_count)(struct block_s *block, struct world_s *world);
} block_t;

extern block_t block_list[256];

block_t block_create(uint8_t id, int texture_id, block_sound_t *sound, float hardness, float resistance, material_t *material);
void block_set_bounds(block_t *block, float x0, float y0, float z0, float x1, float y1, float z1);
uint8_t block_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, int side);
int block_get_texture_id(block_t *block, int face);
void block_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random);
void block_on_neighbor_changed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t block_id);
void block_on_placed(block_t *block, struct world_s *world, int x, int y, int z);
void block_on_added(block_t *block, struct world_s *world, int x, int y, int z);
void block_on_removed(block_t *block, struct world_s *world, int x, int y, int z);
void block_breaking(block_t *block, struct world_s *world, int x, int y, int z, int face, particles_t *particles);
void block_destroy(block_t *block, struct world_s *world, int x, int y, int z, particles_t *particles);
float block_get_brightness(block_t *block, struct world_s *world, int x, int y, int z);
void block_render_inside(block_t *block, int x, int y, int z, int side);
void block_render_side_with_texture(block_t *block, int x, int y, int z, int side, int tex);
void block_render_side(block_t *block, int x, int y, int z, int side);
void block_render_preview(block_t *block);
uint8_t block_render(block_t *block, struct world_s *world, int x, int y, int z);
void block_render_full_brightness(block_t *block);
hit_result_t block_clip(block_t *block, int x, int y, int z, vec3_t v0, vec3_t v1);
AABB_t block_get_selection_aabb(block_t *block, int x, int y, int z);
AABB_t block_get_collision_aabb(block_t *block, int x, int y, int z);
void block_spawn_items(block_t *block, struct world_s *world, int x, int y, int z);
void block_spawn_items_chance(block_t *block, struct world_s *world, int x, int y, int z, float chance);
uint8_t block_get_drop_count(block_t *block, struct world_s *world);