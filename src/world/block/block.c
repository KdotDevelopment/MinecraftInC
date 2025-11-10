#include <world/block/block.h>

#include <item/item.h>
#include <world/world.h>
#include <renderer/tesselator.h>
#include <particle/particle_terrain.h>
#include <entity/entity_item.h>

#include <stdio.h>
#include <stdlib.h>

block_t block_list[256] = { 0 };

block_t block_create(uint8_t id, int texture_id, block_sound_t *sound, float hardness, float resistance, material_t *material) {
    block_t block = { 0 };
    block.id = id;
    block.texture_id = texture_id;
    block.sound = sound;
    block.hardness = hardness;
    block.resistance = resistance ? (resistance * 3) : 5 * hardness;
    block.particle_gravity = 1.0;
    block.material = material;
    block.tick_rate = 5;
    block.light_opacity = id == 0 ? 0 : 255;

    block.is_opaque = texture_id == -1 ? 0 : 1;
    block.is_cube = texture_id == -1 ? 0 : 1;
    block.is_solid = texture_id == -1 ? 0 : 1;
    block.tick_delay = 0;
    block.render_pass = 0;
    block.render_type = BLOCK_RENDER_TYPE_NORMAL;
    block.render_normal_block = 1;
    block.is_collidable = 1;

    block.can_render_side = block_can_render_side;
    block.get_texture_side = block_get_texture_side;
    block.get_texture_side_metadata = block_get_texture_side_metadata;
    block.get_texture = block_get_texture;
    block.update = block_update;
    block.on_neighbor_changed = block_on_neighbor_changed;
    block.on_placed = block_on_placed;
    block.on_added = block_on_added;
    block.on_removed = block_on_removed;
    block.on_destroyed = block_on_destroyed;
    block.get_brightness = block_get_brightness;
    block.render_inside = block_render_inside;
    block.render_preview = block_render_preview;
    block.render = block_render;
    block.render_full_brightness = block_render_full_brightness;
    block.get_selection_aabb = block_get_selection_aabb;
    block.get_collision_aabb = block_get_collision_aabb;
    block.get_drop_count = block_get_drop_count;
    block.on_exploded = block_on_exploded;
    block.visual_update = block_visual_update;
    block.can_place_at = block_can_place_at;
    block.get_item_dropped = block_get_item_dropped;
    block.get_tick_rate = block_get_tick_rate;
    block.get_tile_entity = block_get_tile_entity;
    block.on_interacted = block_on_interacted;
    block.can_grow_on = block_can_grow_on;
    block.can_stay = block_can_stay;
    block.on_walked_upon = block_on_walked_upon;
    block.clip = block_clip;

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
    tesselator_color_opaque(0.5, 0.5, 0.5);
    block->render_inside(block, -2, 0, 0, 0);
    tesselator_color_opaque(1.0, 1.0, 1.0);
    block->render_inside(block, -2, 0, 0, 1);
    tesselator_color_opaque(0.8, 0.8, 0.8);
    block->render_inside(block, -2, 0, 0, 2);
    tesselator_color_opaque(0.8, 0.8, 0.8);
    block->render_inside(block, -2, 0, 0, 3);
    tesselator_color_opaque(0.6, 0.6, 0.6);
    block->render_inside(block, -2, 0, 0, 4);
    tesselator_color_opaque(0.6, 0.6, 0.6);
    block->render_inside(block, -2, 0, 0, 5);
}

float block_get_brightness(block_t *block, struct world_s *world, int x, int y, int z) {
    return world_get_brightness((world_t *)world, x, y, z);
}

uint8_t block_can_render_side(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side) {
    return !world_is_solid((world_t *)world, x, y, z);
}

int block_get_texture_side(block_t *block, uint8_t face) {
    return block->texture_id;
}

int block_get_texture_side_metadata(block_t *block, uint8_t face, uint8_t metadata) {
    return block->get_texture_side(block, face);
}

int block_get_texture(block_t *block, struct world_s *world, int x, int y, int z, uint8_t face) {
    return block->get_texture_side_metadata(block, face, world_get_block_metadata(world, x, y, z));
}

void block_render_inside(block_t *block, int x, int y, int z, uint8_t side) {
    block_render_side_with_texture(block, x, y, z, side, block->get_texture_side(block, side));
}

void block_render_side_with_texture(block_t *block, int x, int y, int z, uint8_t side, int tex) {
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

void block_render_side(block_t *block, int x, int y, int z, uint8_t side) {
    block_render_side_with_texture(block, x, y, z, side, block->get_texture_side(block, side));
}

void block_visual_update(block_t *block, world_t *world, int x, int y, int z, random_t *random) {
    return;
}

AABB_t block_get_selection_aabb(block_t *block, int x, int y, int z) {
    return (AABB_t){ .x0 = x + block->x0, .y0 = y + block->y0, .z0 = z + block->z0, .x1 = x + block->x1, .y1 = y + block->y1, .z1 = z + block->z1 };
}

AABB_t block_get_collision_aabb(block_t *block, int x, int y, int z) {
    return (AABB_t){ .x0 = x + block->x0, .y0 = y + block->y0, .z0 = z + block->z0, .x1 = x + block->x1, .y1 = y + block->y1, .z1 = z + block->z1 };
}

void block_update(block_t *block, struct world_s *world, int x, int y, int z, random_t *random) {
    return;
}

void block_on_neighbor_changed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t block_id) {
    return;
}

void block_on_placed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t side) {
    return;
}

void block_on_added(block_t *block, struct world_s *world, int x, int y, int z) {
    return;
}

void block_on_removed(block_t *block, struct world_s *world, int x, int y, int z) {
    return;
}

void block_breaking(block_t *block, struct world_s *world, int x, int y, int z, uint8_t face, particles_t *particles) {
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
    *particle = particle_terrain_create(world, xx, yy, zz, 0.0, 0.0, 0.0, block);
    particle_set_scale(particle, 0.6);
    particle_set_power(particle, 0.2);
    particles_spawn_particle(particles, particle);
}

void block_on_destroyed(block_t *block, struct world_s *world, int x, int y, int z, uint8_t metadata) {
    return;
}

void block_render_preview(block_t *block) {
    tesselator_begin_quads();
    for(int i = 0; i < 6; i++) {
        if(i == 0) tesselator_normal(0.0, 1.0, 0.0);
        if(i == 1) tesselator_normal(0.0, -1.0, 0.0);
        if(i == 2) tesselator_normal(0.0, 0.0, 1.0);
        if(i == 3) tesselator_normal(0.0, 0.0, -1.0);
        if(i == 4) tesselator_normal(1.0, 0.0, 0.0);
        if(i == 5) tesselator_normal(-1.0, 0.0, 0.0);
        block->render_inside(block, 0, 0, 0, i);
    }
    tesselator_end();
}

hit_result_t block_clip(block_t *block, world_t *world, int x, int y, int z, vec3_t v0, vec3_t v1) {
    hit_result_t pos = AABB_clip((AABB_t){ block->x0, block->y0, block->z0, block->x1, block->y1, block->z1 }, vec3_subtract(v0, (vec3_t){ x, y, z }), vec3_subtract(v1, (vec3_t){ x, y, z }));
    if(!pos.null) {
        pos.x = x;
        pos.y = y;
        pos.z = z;
        pos.location = vec3_add(pos.location, (vec3_t){ x, y, z });
    }
    return pos;
}

uint8_t block_render(block_t *block, struct world_s *world, int x, int y, int z) {
    uint8_t rendered = 0;
    if(block->can_render_side(block, world, x, y - 1, z, 0)) {
        float brightness = block->get_brightness(block, world, x, y - 1, z);
        tesselator_color_opaque(0.5 * brightness, 0.5 * brightness, 0.5 * brightness);
        block->render_inside(block, x, y, z, 0);
        rendered = 1;
    }
    if(block->can_render_side(block, world, x, y + 1, z, 1)) {
        float brightness = block->get_brightness(block, world, x, y + 1, z);
        tesselator_color_opaque(brightness, brightness, brightness);
        block->render_inside(block, x, y, z, 1);
        rendered = 1;
    }
    if(block->can_render_side(block, world, x, y, z - 1, 2)) {
        float brightness = block->get_brightness(block, world, x, y, z - 1);
        tesselator_color_opaque(0.8 * brightness, 0.8 * brightness, 0.8 * brightness);
        block->render_inside(block, x, y, z, 2);
        rendered = 1;
    }
    if(block->can_render_side(block, world, x, y, z + 1, 3)) {
        float brightness = block->get_brightness(block, world, x, y, z + 1);
        tesselator_color_opaque(0.8 * brightness, 0.8 * brightness, 0.8 * brightness);
        block->render_inside(block, x, y, z, 3);
        rendered = 1;
    }
    if(block->can_render_side(block, world, x - 1, y, z, 4)) {
        float brightness = block->get_brightness(block, world, x - 1, y, z);
        tesselator_color_opaque(0.6 * brightness, 0.6 * brightness, 0.6 * brightness);
        block->render_inside(block, x, y, z, 4);
        rendered = 1;
    }
    if(block->can_render_side(block, world, x + 1, y, z, 5)) {
        float brightness = block->get_brightness(block, world, x + 1, y, z);
        tesselator_color_opaque(0.6 * brightness, 0.6 * brightness, 0.6 * brightness);
        block->render_inside(block, x, y, z, 5);
        rendered = 1;
    }

    return rendered;
}

int16_t block_get_item_dropped(block_t *block, uint8_t metadata, random_t *random) {
    return block->id;
}

void block_spawn_items(block_t *block, world_t *world, int x, int y, int z, uint8_t metadata) {
    block_spawn_items_chance(block, world, x, y, z, metadata, 1.0);
}

void block_spawn_items_chance(block_t *block, world_t *world, int x, int y, int z, uint8_t metadata, float chance) {
    uint8_t count = block->get_drop_count(block, &world->random);

    for(int i = 0; i < count; i++) {
        if(random_next_uniform(&world->random) <= chance) {
            int16_t item_id = block->get_item_dropped(block, metadata, &world->random);
            if(item_id < 0) continue;
            float x_diff = random_next_uniform(&world->random) * 0.7 + 0.15;
            float y_diff = random_next_uniform(&world->random) * 0.7 + 0.15;
            float z_diff = random_next_uniform(&world->random) * 0.7 + 0.15;
            entity_t *item = malloc(sizeof(entity_t));
            item_stack_t item_stack = item_stack_create(item_id, 1, 0);
            entity_item_create(item, world, x + x_diff, y + y_diff, z + z_diff, item_stack);
            item->delay_before_pickup = 10;
            world_spawn_entity(world, item);
        }
    }
}

float block_get_strength(block_t *block, entity_t *player) {
    if(block->hardness < 0) {
        return 0;
    }else if(!player_can_harvest_block(player, block)) {
        return 1.0 / block->hardness / 100.0;
    }else {
        inventory_t *inv = &player->mob->player->inventory;
        float multiplier = 1.0;
        if(inv->inv[inv->selected].item_id != 0) {
            item_stack_t item = inv->inv[inv->selected];
            multiplier = 1.0 * item_list[item.item_id].get_strength_against_block(&item_list[item.item_id], block);
        }

        float speed = multiplier;
        if(entity_is_underwater(player)) {
            speed = multiplier / 5.0;
        }

        if(!entity_on_ground(player)) {
            speed /= 5.0;
        }

        return speed / block->hardness / 30.0;
    }
}

uint8_t block_get_drop_count(block_t *block, random_t *random) {
    return 1;
}

uint8_t block_can_place_at(block_t *block, world_t *world, int x, int y, int z) {
    return 1;
}

void block_on_exploded(block_t *block, world_t *world, int x, int y, int z) {
    return;
}

int block_get_tick_rate(block_t *block) {
    return 5;
}

tile_entity_t *block_get_tile_entity(world_t *world) {
    return NULL;
}

uint8_t block_on_interacted(block_t *block, struct world_s *world, int x, int y, int z, struct entity_s *player) {
    return 0;
}

uint8_t block_can_grow_on(block_t *block, uint8_t block_id) {
    return 0;
}

uint8_t block_can_stay(block_t *block, world_t *world, int x, int y, int z) {
    return 1;
}

void block_on_walked_upon(block_t *block, struct world_s *world, int x, int y, int z) {
    return;
}