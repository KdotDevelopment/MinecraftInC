#include <level/block/block.h>
#include <level/level.h>
#include <renderer/tesselator.h>
#include <particle/particle_terrain.h>

#include <stdio.h>
#include <stdlib.h>

block_t block_list[256] = { 0 };

block_t block_create(uint8_t id, int texture_id, block_sound_t sound, float particle_gravity, float break_speed, uint8_t can_explode) {
    block_t block = { 0 };
    block.id = id;
    block.texture_id = texture_id;
    block.sound = sound;
    block.explodable = can_explode;
    block.particle_gravity = particle_gravity;
    block.destroy_speed = 20 * break_speed;

    block.is_opaque = texture_id == -1 ? 0 : 1;
    block.is_cube = texture_id == -1 ? 0 : 1;
    block.is_solid = texture_id == -1 ? 0 : 1;
    block.liquid_type = LIQUID_NONE;
    block.tick_delay = 0;
    block.render_pass = 0;

    block.can_render_side = block_can_render_side;
    block.get_texture_id = block_get_texture_id;
    block.update = block_update;
    block.on_neighbor_changed = block_on_neighbor_changed;
    block.on_placed = block_on_placed;
    block.on_added = block_on_added;
    block.on_removed = block_on_removed;
    block.destroy = block_destroy;
    block.get_brightness = block_get_brightness;
    block.render_inside = block_render_inside;
    block.render_preview = block_render_preview;
    block.render = block_render;
    block.render_full_brightness = block_render_full_brightness;
    block.get_selection_aabb = block_get_selection_aabb;
    block.get_collision_aabb = block_get_collision_aabb;

    block_set_bounds(&block, 0, 0, 0, 1, 1, 1);

    block_list[id] = block;

    return block;
}

void block_set_bounds(block_t *block, float x0, float y0, float z0, float x1, float y1, float z1) {
    block->x0 = x0;
    block->y0 = y0;
    block->z0 = z0;
    block->x1 = x1;
    block->y1 = y1;
    block->z1 = z1;
}

void block_render_full_brightness(block_t *block) {
    tesselator_color(0.5, 0.5, 0.5);
    block->render_inside(block, -2, 0, 0, 0);
    tesselator_color(1.0, 1.0, 1.0);
    block->render_inside(block, -2, 0, 0, 1);
    tesselator_color(0.8, 0.8, 0.8);
    block->render_inside(block, -2, 0, 0, 2);
    tesselator_color(0.8, 0.8, 0.8);
    block->render_inside(block, -2, 0, 0, 3);
    tesselator_color(0.6, 0.6, 0.6);
    block->render_inside(block, -2, 0, 0, 4);
    tesselator_color(0.6, 0.6, 0.6);
    block->render_inside(block, -2, 0, 0, 5);
}

float block_get_brightness(block_t *block, struct level_s *level, int x, int y, int z) {
    return level_get_brightness((level_t *)level, x, y, z);
}

uint8_t block_can_render_side(block_t *block, struct level_s *level, int x, int y, int z, int side) {
    return !level_is_solid_block((level_t *)level, x, y, z);
}

int block_get_texture_id(block_t *block, int face) {
    return block->texture_id;
}

void block_render_inside(block_t *block, int x, int y, int z, int side) {
    block_render_side_with_texture(block, x, y, z, side, block->get_texture_id(block, side));
}

void block_render_side_with_texture(block_t *block, int x, int y, int z, int side, int tex) {
    int u0 = (tex % 16) << 4;
    int v0 = (tex / 16) << 4;
    float u1 = u0 / 256.0;
    float v1 = v0 / 256.0;
    float u2 = (u0 + 15.99) / 256.0;
    float v2 = (v0 + 15.99) / 256.0;

    if(side >= 2 && tex < 240) {
        if(block->y0 >= 0 && block->y1 <= 1) {
            v1 = (v0 + block->y0 * 15.99) / 256.0;
            v2 = (v0 + block->y1 * 15.99) / 256.0;
        }else {
            v1 = v0 / 256.0;
            v2 = (v0 + 15.99) / 256.0;
        }
    }

    float x0 = x + block->x0;
    float y0 = y + block->y0;
    float z0 = z + block->z0;
    float x1 = x + block->x1;
    float y1 = y + block->y1;
    float z1 = z + block->z1;

    switch(side) {
        case 0:
            tesselator_vertex_uv(x0, y0, z1, u1, v2);
            tesselator_vertex_uv(x0, y0, z0, u1, v1);
            tesselator_vertex_uv(x1, y0, z0, u2, v1);
            tesselator_vertex_uv(x1, y0, z1, u2, v2);
            break;
        case 1:
            tesselator_vertex_uv(x1, y1, z1, u2, v2);
            tesselator_vertex_uv(x1, y1, z0, u2, v1);
            tesselator_vertex_uv(x0, y1, z0, u1, v1);
            tesselator_vertex_uv(x0, y1, z1, u1, v2);
            break;
        case 2:
            tesselator_vertex_uv(x0, y1, z0, u2, v1);
            tesselator_vertex_uv(x1, y1, z0, u1, v1);
            tesselator_vertex_uv(x1, y0, z0, u1, v2);
            tesselator_vertex_uv(x0, y0, z0, u2, v2);
            break;
        case 3:
            tesselator_vertex_uv(x0, y1, z1, u1, v1);
            tesselator_vertex_uv(x0, y0, z1, u1, v2);
            tesselator_vertex_uv(x1, y0, z1, u2, v2);
            tesselator_vertex_uv(x1, y1, z1, u2, v1);
            break;
        case 4:
            tesselator_vertex_uv(x0, y1, z1, u2, v1);
            tesselator_vertex_uv(x0, y1, z0, u1, v1);
            tesselator_vertex_uv(x0, y0, z0, u1, v2);
            tesselator_vertex_uv(x0, y0, z1, u2, v2);
            break;
        case 5:
            tesselator_vertex_uv(x1, y0, z1, u1, v2);
            tesselator_vertex_uv(x1, y0, z0, u2, v2);
            tesselator_vertex_uv(x1, y1, z0, u2, v1);
            tesselator_vertex_uv(x1, y1, z1, u1, v1);
            break;
    }
}

void block_render_side(block_t *block, int x, int y, int z, int side) {
    block_render_side_with_texture(block, x, y, z, side, block->get_texture_id(block, side));
}

AABB_t block_get_selection_aabb(block_t *block, int x, int y, int z) {
    return (AABB_t){ .x0 = x + block->x0, .y0 = y + block->y0, .z0 = z + block->z0, .x1 = x + block->x1, .y1 = y + block->y1, .z1 = z + block->z1 };
}

AABB_t block_get_collision_aabb(block_t *block, int x, int y, int z) {
    return (AABB_t){ .x0 = x + block->x0, .y0 = y + block->y0, .z0 = z + block->z0, .x1 = x + block->x1, .y1 = y + block->y1, .z1 = z + block->z1 };
}

void block_update(block_t *block, struct level_s *level, int x, int y, int z, random_t *random) {
    return;
}

void block_on_neighbor_changed(block_t *block, struct level_s *level, int x, int y, int z, uint8_t block_id) {
    return;
}

void block_on_placed(block_t *block, struct level_s *level, int x, int y, int z) {
    return;
}

void block_on_added(block_t *block, struct level_s *level, int x, int y, int z) {
    return;
}

void block_on_removed(block_t *block, struct level_s *level, int x, int y, int z) {
    return;
}

void block_breaking(block_t *block, struct level_s *level, int x, int y, int z, int face, particles_t *particles) {
    float a = 0.1;
    float xx = x + random_uniform() * (block->x1 - block->x0 - a * 2.0) + a + block->x0;
    float yy = y + random_uniform() * (block->y1 - block->y0 - a * 2.0) + a + block->y0;
    float zz = z + random_uniform() * (block->z1 - block->z0 - a * 2.0) + a + block->z0;
    if(face == 0) {
        yy = y + block->y0 - a;
    }
    if(face == 1) {
        yy = y + block->y1 + a;
    }
    if(face == 2) {
        zz = z + block->z0 - a;
    }
    if(face == 3) {
        zz = z + block->z1 + a;
    }
    if(face == 4) {
        xx = x + block->x0 - a;
    }
    if(face == 5) {
        xx = x + block->x1 + a;
    }

    particle_t *particle = malloc(sizeof(particle_t));
    *particle = particle_terrain_create(level, xx, yy, zz, 0.0, 0.0, 0.0, block);
    particle_set_scale(particle, 0.6);
    particle_set_power(particle, 0.2);
    particles_spawn_particle(particles, particle);
}

void block_destroy(block_t *block, struct level_s *level, int x, int y, int z, particles_t *particles) {
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            for(int k = 0; k < 4; k++) {
                float xd = x + (i + 0.5) / 4.0;
                float yd = y + (j + 0.5) / 4.0;
                float zd = z + (k + 0.5) / 4.0;
                particle_t *particle = malloc(sizeof(particle_t));
                *particle = particle_terrain_create(level, xd, yd, zd, xd - x - 0.5, yd - y - 0.5, zd - z - 0.5, block);
                particles_spawn_particle(particles, particle);
            }
        }
    }
}

void block_render_preview(block_t *block) {
    tesselator_begin();
    for(int i = 0; i < 6; i++) {
        if (i == 0) tesselator_normal(0.0, 1.0, 0.0);
        if (i == 1) tesselator_normal(0.0, -1.0, 0.0);
        if (i == 2) tesselator_normal(0.0, 0.0, 1.0);
        if (i == 3) tesselator_normal(0.0, 0.0, -1.0);
        if (i == 4) tesselator_normal(1.0, 0.0, 0.0);
        if (i == 5) tesselator_normal(-1.0, 0.0, 0.0);
        block->render_inside(block, 0, 0, 0, i);
    }
    tesselator_end();
}

hit_result_t block_clip(block_t *block, int x, int y, int z, vec3_t v0, vec3_t v1) {
    hit_result_t pos = AABB_clip((AABB_t){ block->x0, block->y0, block->z0, block->x1, block->y1, block->z1 }, vec3_subtract(v0, (vec3_t){ x, y, z }), vec3_subtract(v1, (vec3_t){ x, y, z }));
    if (!pos.null) {
        pos.x = x;
        pos.y = y;
        pos.z = z;
        pos.location = vec3_add(pos.location, (vec3_t){ x, y, z });
    }
    return pos;
}

uint8_t block_render(block_t *block, struct level_s *level, int x, int y, int z) {
    uint8_t rendered = 0;
    if(block->can_render_side(block, level, x, y - 1, z, 0)) {
        float brightness = block->get_brightness(block, level, x, y - 1, z);
        tesselator_color(0.5 * brightness, 0.5 * brightness, 0.5 * brightness);
        block->render_inside(block, x, y, z, 0);
        rendered = 1;
    }
    if(block->can_render_side(block, level, x, y + 1, z, 1)) {
        float brightness = block->get_brightness(block, level, x, y + 1, z);
        tesselator_color(brightness, brightness, brightness);
        block->render_inside(block, x, y, z, 1);
        rendered = 1;
    }
    if(block->can_render_side(block, level, x, y, z - 1, 2)) {
        float brightness = block->get_brightness(block, level, x, y, z - 1);
        tesselator_color(0.8 * brightness, 0.8 * brightness, 0.8 * brightness);
        block->render_inside(block, x, y, z, 2);
        rendered = 1;
    }
    if(block->can_render_side(block, level, x, y, z + 1, 3)) {
        float brightness = block->get_brightness(block, level, x, y, z + 1);
        tesselator_color(0.8 * brightness, 0.8 * brightness, 0.8 * brightness);
        block->render_inside(block, x, y, z, 3);
        rendered = 1;
    }
    if(block->can_render_side(block, level, x - 1, y, z, 4)) {
        float brightness = block->get_brightness(block, level, x - 1, y, z);
        tesselator_color(0.6 * brightness, 0.6 * brightness, 0.6 * brightness);
        block->render_inside(block, x, y, z, 4);
        rendered = 1;
    }
    if(block->can_render_side(block, level, x + 1, y, z, 5)) {
        float brightness = block->get_brightness(block, level, x + 1, y, z);
        tesselator_color(0.6 * brightness, 0.6 * brightness, 0.6 * brightness);
        block->render_inside(block, x, y, z, 5);
        rendered = 1;
    }

    return rendered;
}