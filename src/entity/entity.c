#include <entity/entity.h>
#include <level/level.h>
#include <physics/AABB.h>
#include <util/array_list.h>
#include <level/block/block.h>
#include <level/block/blocks.h>

#include <math.h>
#include <stdio.h>

void entity_create(entity_t *entity, struct level_s *level) {
    //entity_t entity = { 0 };
    entity->level = level;
    //set_pos
    entity->on_ground = 0;
    entity->horizontal_collision = 0;
    entity->collision = 0;
    entity->slide = 1;
    entity->removed = 0;
    entity->height_offset = 0.0F;
    entity->bb_width = 0.6F;
    entity->bb_height = 1.8F;
    entity->walk_disto = 0.0F;
    entity->walk_dist = 0.0F;
    entity->make_step_sound = 1;
    entity->fall_distance = 0.0F;
    entity->next_step = 1;
    entity->texture_id = 0;
    entity->y_slide_offset = 0.0F;
    entity->foot_size = 0.0F;
    entity->no_physics = 0;
    entity->push_through = 0.0F;
    entity->hovered = 0;
    entity->is_shootable = 0;

    entity->tick = entity_tick;
    entity->render = entity_render;
    entity->hurt = entity_hurt;
    entity->cause_fall_damage = entity_cause_fall_damage;
    entity->award_kill_score = entity_award_kill_score;
    entity->get_brightness = entity_get_brightness;
    entity->heal = entity_heal;
    entity->can_be_hit = entity_can_be_hit;

    entity_set_pos(entity, 0, 0, 0);

    //return entity;
}

void entity_set_pos(entity_t *entity, float x, float y, float z) {
    entity->x = x;
    entity->y = y;
    entity->z = z;
    float width = entity->bb_width / 2.0;
    float height = entity->bb_height / 2.0;
    entity->bb = (AABB_t){x - width, y - height, z - width, x + width, y + height, z + width};
}

void entity_reset_pos(entity_t *entity) {
    if(!entity->level) return;
    level_t *level = (level_t *)entity->level;
    float x = level->spawn_x + 0.5;
    float y = level->spawn_y;
    float z = level->spawn_z + 0.5;

    for(; z >  0; z++) {
        entity_set_pos(entity, x, y, z);
        AABB_t *cubes = level_get_cubes((level_t *)entity->level, entity->bb);
        if(array_list_length(cubes) == 0) {
            array_list_free(cubes);
            break;
        }
        array_list_free(cubes);
    }

    entity->xd = entity->yd = entity->zd = 0;
    entity->y_rot = level->spawn_rot;
    entity->x_rot = 0;
}

void entity_turn(entity_t *entity, float y_rot, float x_rot) {
    float curr_x_rot = entity->x_rot;
    float curr_y_rot = entity->y_rot;
    entity->y_rot = (float)((double)entity->y_rot + (double)x_rot * 0.15);
    entity->x_rot = (float)((double)entity->x_rot - (double)y_rot * 0.15);
    if (entity->x_rot < -90.0F) {
        entity->x_rot = -90.0F;
    }

    if (entity->x_rot > 90.0F) {
        entity->x_rot = 90.0F;
    }

    entity->x_roto = entity->x_roto + (entity->x_rot - curr_x_rot);
    entity->y_roto = entity->y_roto + (entity->y_rot - curr_y_rot);
}

void entity_interpolate_turn(entity_t *entity, float y_rot, float x_rot) {
    entity->y_rot = (float)((double)entity->y_rot + (double)y_rot * 0.15);
    entity->x_rot = (float)((double)entity->x_rot - (double)x_rot * 0.15);
    if (entity->x_rot < -90.0F) {
        entity->x_rot = -90.0F;
    }

    if (entity->x_rot > 90.0F) {
        entity->x_rot = 90.0F;
    }
}

void entity_tick(entity_t *entity) {
    entity->walk_disto = entity->walk_dist;
    entity->xo = entity->x;
    entity->yo = entity->y;
    entity->zo = entity->z;
    entity->x_roto = entity->x_rot;
    entity->y_roto = entity->y_rot;
}

void entity_play_sound(entity_t *entity, char *sound, float volume, float pitch) {
    level_play_sound((level_t *)entity->level, sound, entity, volume, pitch);
}

void entity_move(entity_t *entity, float x, float y, float z) {
    if(entity->no_physics) {
        AABB_move(entity->bb, x, y, z);
        entity->x = (entity->bb.x0 + entity->bb.x1) / 2.0;
        entity->y = entity->bb.y0 + entity->height_offset - entity->y_slide_offset;
        entity->z = (entity->bb.z0 + entity->bb.z1) / 2.0;
        return;
    }

    float x1 = entity->x;
    float z1 = entity->z;
    float ox = x;
    float oy = y;
    float oz = z;

    AABB_t oaabb = entity->bb;
    AABB_t *cubes = level_get_cubes((level_t *)entity->level, AABB_expand(entity->bb, x, y, z));

    for(int i = 0; i < array_list_length(cubes); i++) {
        AABB_t aabb = { cubes[i].x0, cubes[i].y0, cubes[i].z0, cubes[i].x1, cubes[i].y1, cubes[i].z1 };
        y = AABB_clip_y_collide(aabb, entity->bb, y);
    }
    entity->bb = AABB_move(entity->bb, 0, y, 0);
    if(!entity->slide && oy != y) {
        x = 0;
        y = 0;
        z = 0;
    }
    for(int i = 0; i < array_list_length(cubes); i++) {
        AABB_t aabb = { cubes[i].x0, cubes[i].y0, cubes[i].z0, cubes[i].x1, cubes[i].y1, cubes[i].z1 };
        x = AABB_clip_x_collide(aabb, entity->bb, x);
    }
    entity->bb = AABB_move(entity->bb, x, 0, 0);
    if(!entity->slide && ox != x) {
        x = 0;
        y = 0;
        z = 0;
    }
    for(int i = 0; i < array_list_length(cubes); i++) {
        AABB_t aabb = { cubes[i].x0, cubes[i].y0, cubes[i].z0, cubes[i].x1, cubes[i].y1, cubes[i].z1 };
        z = AABB_clip_z_collide(aabb, entity->bb, z);
    }
    entity->bb = AABB_move(entity->bb, 0, 0, z);
    if(!entity->slide && oz != z) {
        x = 0;
        y = 0;
        z = 0;
    }

    uint8_t on_ground = entity->on_ground || (oy != y && oy < 0);
    array_list_free(cubes);

    if(entity->foot_size > 0 && on_ground && entity->y_slide_offset < 0.05 && (ox != x || ox != z)) {
        float bx = x;
        float by = y;
        float bz = z;
        x = ox;
        y = entity->foot_size;
        z = oz;
        AABB_t temp_bb = entity->bb;
        entity->bb = oaabb;
        AABB_t *cubes = level_get_cubes((level_t *)entity->level, AABB_expand(entity->bb, x, y, z));

        for(int i = 0; i < array_list_length(cubes); i++) {
            AABB_t aabb = { cubes[i].x0, cubes[i].y0, cubes[i].z0, cubes[i].x1, cubes[i].y1, cubes[i].z1 };
            y = AABB_clip_y_collide(aabb, entity->bb, y);
        }
        entity->bb = AABB_move(entity->bb, 0, y, 0);
        if(!entity->slide && oy != y) {
            x = 0;
            y = 0;
            z = 0;
        }
        for(int i = 0; i < array_list_length(cubes); i++) {
            AABB_t aabb = { cubes[i].x0, cubes[i].y0, cubes[i].z0, cubes[i].x1, cubes[i].y1, cubes[i].z1 };
            x = AABB_clip_x_collide(aabb, entity->bb, x);
        }
        entity->bb = AABB_move(entity->bb, x, 0, 0);
        if(!entity->slide && ox != x) {
            x = 0;
            y = 0;
            z = 0;
        }
        for(int i = 0; i < array_list_length(cubes); i++) {
            AABB_t aabb = { cubes[i].x0, cubes[i].y0, cubes[i].z0, cubes[i].x1, cubes[i].y1, cubes[i].z1 };
            z = AABB_clip_z_collide(aabb, entity->bb, z);
        }
        entity->bb = AABB_move(entity->bb, 0, 0, z);
        if(!entity->slide && oz != z) {
            x = 0;
            y = 0;
            z = 0;
        }
        array_list_free(cubes);

        if(bx * bx + bz * bz >= x * x + z * z) {
            x = bx;
            y = by;
            z = bz;
            entity->bb = temp_bb;
        }else {
            entity->y_slide_offset += 0.5;
        }
    }

    entity->horizontal_collision = ox != x || oz != z;
    entity->on_ground = oy != y && oy < 0.0;
    entity->collision = entity->horizontal_collision || oy != y;
    if(entity->on_ground) {
        if(entity->fall_distance > 0) {
            entity->cause_fall_damage(entity, entity->fall_distance);
            entity->fall_distance = 0;
        }
    }else if(y < 0) {
        entity->fall_distance -= y;
    }

    if(ox != x) entity->xd = 0;
    if(oy != y) entity->yd = 0;
    if(oz != z) entity->zd = 0;

    entity->x = (entity->bb.x0 + entity->bb.x1) / 2.0;
    entity->y = entity->bb.y0 + entity->height_offset - entity->y_slide_offset;
    entity->z = (entity->bb.z0 + entity->bb.z1) / 2.0;
    entity->walk_dist += sqrtf((entity->x - x1) * (entity->x - x1) + (entity->z - z1) * (entity->z - z1)) * 0.6;
    if(entity->make_step_sound) {
        uint8_t block_id = level_get_block((level_t *)entity->level, entity->x, entity->y - entity->height_offset - 0.2, entity->z);
        if(entity->walk_dist > entity->next_step && block_id > blocks.air.id) {
            entity->next_step++;
            block_sound_t sound = block_list[block_id].sound;
            if(sound.type != BLOCK_SOUND_NONE) {
                entity_play_sound(entity, sound.name, sound.volume, sound.pitch);
            }
        }
    }
    entity->y_slide_offset *= 0.4;
}

void entity_move_to(entity_t *entity, float x, float y, float z, float x_rot, float y_rot) {
    entity->xo = entity->x = x;
    entity->yo = entity->y = y;
    entity->zo = entity->z = z;
    entity->x_rot = x_rot;
    entity->y_rot = y_rot;
    entity_set_pos(entity, x, y, z);
}

void entity_move_relative(entity_t *entity, float x, float z, float speed) {
    float distance = sqrtf(x * x + z * z);
    if(distance < 0.01) return;
    if(distance < 1.0) distance = 1.0;
    distance = speed / distance;
    x *= distance;
    z *= distance;
    speed = tsin(entity->y_rot * M_PI / 180);
    distance = tcos(entity->y_rot * M_PI / 180);
    entity->xd += x * distance - z * speed;
    entity->zd += z * distance + x * speed;
}

float entity_distance_to(entity_t *a, entity_t *b) {
    float x = b->x - a->x;
    float y = b->y - a->y;
    float z = b->z - a->z;
    return sqrtf(x * x + y * y + z * z);
}

float entity_distance_to_sqr(entity_t *a, entity_t *b) {
    float x = b->x - a->x;
    float y = b->y - a->y;
    float z = b->z - a->z;
    return x * x + y * y + z * z;
}

void entity_push_dist(entity_t *entity, float x, float y, float z) {
    entity->xd += x;
    entity->yd += y;
    entity->zd += z;
}

void entity_push(entity_t *a, entity_t *b) {
    float x_diff = b->x - a->x;
    float z_diff = b->z - a->z;
    float distance = x_diff * x_diff + z_diff * z_diff;
    if(distance < 0.01) return;
    
    distance = sqrt(distance);
    x_diff /= distance;
    z_diff /= distance;
    x_diff /= distance;
    z_diff /= distance;
    x_diff *= 0.05;
    z_diff *= 0.05;
    x_diff *= 1.0 - a->push_through;
    z_diff *= 1.0 - a->push_through;
    entity_push_dist(a, -x_diff, 0, -z_diff);
    entity_push_dist(b, x_diff, 0, z_diff);
}

uint8_t should_render_at_sqr_distance(entity_t *entity, float distance) {
    float size = AABB_get_size(entity->bb) * 64;
    return distance < size * size;
}

uint8_t entity_should_render(entity_t *entity, vec3_t vec) {
    float x = entity->x - vec.x;
    float y = entity->y - vec.y;
    float z = entity->z - vec.z;
    return should_render_at_sqr_distance(entity, x * x + y * y + z * z);
}

void entity_remove(entity_t *entity) {
    entity->removed = 1;
}

uint8_t entity_is_free(entity_t *entity, float x, float y, float z) {
    AABB_t bb = AABB_move(entity->bb, x, y, z);
    AABB_t *cubes = level_get_cubes((level_t *)entity->level, bb);
    uint8_t free = array_list_length(cubes) > 0 ? 0 : !level_contains_any_liquid((level_t *)entity->level, bb);
    array_list_free(cubes);
    return free;
}

uint8_t entity_is_underwater(entity_t *entity) {
    uint8_t block_id = level_get_block((level_t *)entity->level, entity->x, entity->y, entity->z);
    return block_id != 0 ? block_list[block_id].liquid_type == LIQUID_WATER : 0;
}

uint8_t entity_is_in_water(entity_t *entity) {
    return level_contains_liquid((level_t *)entity->level, AABB_grow(entity->bb, 0, -0.4, 0), LIQUID_WATER);
}

uint8_t entity_is_in_lava(entity_t *entity) {
    return level_contains_liquid((level_t *)entity->level, AABB_grow(entity->bb, 0, -0.4, 0), LIQUID_LAVA);
}

uint8_t entity_on_ground(entity_t *entity) {
    return entity->on_ground;
}

float entity_get_brightness(entity_t *entity, float tick) {
    return level_get_brightness((level_t *)entity->level, entity->x, entity->y + entity->height_offset / 2.0 - 0.5, entity->z);
}

uint8_t entity_can_be_hit(entity_t *entity) {
    return 0;
}

void entity_cause_fall_damage(entity_t *entity, float distance) {
}

void entity_hurt(entity_t *entity, entity_t *causer, int damage) {
}

void entity_heal(struct entity_s *entity, int damage) {
}

void entity_render(entity_t *entity, textures_t *textures, float delta) {
}

void entity_award_kill_score(entity_t *entity, entity_t *causer, int score) {
}

void entity_player_touch(entity_t *entity, entity_t *player) {
}