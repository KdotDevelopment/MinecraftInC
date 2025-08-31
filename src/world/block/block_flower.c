#include <world/block/block_flower.h>

#include <world/block/blocks.h>
#include <world/block/block_sound.h>
#include <world/world.h>
#include <world/material/materials.h>
#include <renderer/tesselator.h>

#include <math.h>

block_t block_flower_create(uint8_t id, int texture_id) {
    block_t block = block_create(id, texture_id, &block_sounds.none, 0, 0, &materials.plants);
    block.is_cube = 0;
    block.is_opaque = 0;
    block.is_solid = 0;
    block.has_physics = 1;
    block.should_tick = 1;
    block.render_type = BLOCK_RENDER_TYPE_FLOWER;

    block.update = block_flower_update;
    block.get_collision_aabb = block_flower_get_collision_aabb;
    block.render_preview = block_flower_render_preview;
    block.render = block_flower_render;
    block.render_full_brightness = block_flower_render_full_brightness;
    block.can_grow_on = block_flower_can_grow_on;
    block.can_stay = block_flower_can_stay;

    float w = 0.2;
    block_set_bounds(&block, 0.5 - w, 0, 0.5 - w, 0.5 + w, 3.0 * w, 0.5 + w);

    block_list[block.id] = block;

    return block;
}

uint8_t block_flower_can_grow_on(block_t *block, uint8_t block_id) {
    return block_id == BLOCK_GRASS || block_id == BLOCK_DIRT || block_id == BLOCK_FARMLAND;
}

uint8_t block_flower_can_stay(block_t *block, world_t *world, int x, int y, int z) {
    return (world_get_block_light_value(world, x, y, z) >= 8 || world_can_block_see_sky(world, x, y, z)) && block->can_grow_on(block, world_get_block(world, x, y - 1, z));
}

void private_check_flower_change(block_t *block, world_t *world, int x, int y, int z) {
    if(!block->can_stay(block, world, x, y, z)) {
        block_spawn_items(block, world, x, y, z, world_get_block_metadata(world, x, y, z));
        world_set_block_with_update(world, x, y, z, blocks.air.id);
    }
}

void block_flower_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    private_check_flower_change(block, world, x, y, z);
}

AABB_t block_flower_get_collision_aabb(block_t *block, int x, int y, int z) {
    return (AABB_t){ .null = 1 };
}

void private_block_flower_render(block_t *block, float x, float y, float z) {
    int tex = block->get_texture_side(block, 15);
    int u0 = (tex % 16) << 4;
    int v0 = (tex / 16) << 4;
    float u1 = u0 / 256.0;
    float v1 = v0 / 256.0;
    float u2 = (u0 + 15.99) / 256.0;
    float v2 = (v0 + 15.99) / 256.0;
    for (int i = 0; i < 2; i++) {
        float s = tsin(i * M_PI / 2.0 + M_PI / 4.0) * 0.5;
        float c = tcos(i * M_PI / 2.0 + M_PI / 4.0) * 0.5;
        float x1 = x + 0.5 - s;
        float y1 = y + 1.0;
        float z1 = z + 0.5 - c;
        s += x + 0.5;
        c += z + 0.5;
        tesselator_vertex_uv(x1, y1, z1, u2, v1);
        tesselator_vertex_uv(s, y1, c, u1, v1);
        tesselator_vertex_uv(s, y, c, u1, v2);
        tesselator_vertex_uv(x1, y, z1, u2, v2);
        tesselator_vertex_uv(s, y1, c, u2, v1);
        tesselator_vertex_uv(x1, y1, z1, u1, v1);
        tesselator_vertex_uv(x1, y, z1, u1, v2);
        tesselator_vertex_uv(s, y, c, u2, v2);
    }
}

void block_flower_render_preview(block_t *block) {
    tesselator_normal(0.0, 1.0, 0.0);
    tesselator_begin_quads();
    private_block_flower_render(block, 0, 0.4, -0.3);
    tesselator_end();
}

uint8_t block_flower_render(block_t *block, world_t *world, int x, int y, int z) {
    float brightness = world_get_brightness(world, x, y, z);
    tesselator_color_opaque(brightness, brightness, brightness);
    private_block_flower_render(block, x, y, z);
    return 1;
}

void block_flower_render_full_brightness(block_t *block) {
    tesselator_color_opaque(1, 1, 1);
    private_block_flower_render(block, -2, 0, 0);
}