#include <entity/entity.h>

#include <entity/entity_arrow.h>
#include <entity/entity_item.h>
#include <entity/entity_painting.h>
#include <entity/entity_primed_tnt.h>
#include <entity/mob/mob.h>
#include <entity/mob/mob_creeper.h>
#include <entity/mob/mob_giant.h>
#include <entity/mob/mob_pig.h>
#include <entity/mob/mob_sheep.h>
#include <entity/mob/mob_skeleton.h>
#include <entity/mob/mob_spider.h>
#include <entity/mob/mob_zombie.h>
#include <nbt/nbt_tag_list.h>
#include <nbt/nbt_tag_double.h>
#include <nbt/nbt_tag_float.h>
#include <particle/particle_bubble.h>
#include <particle/particle_splash.h>
#include <physics/AABB.h>
#include <sound/sounds.h>
#include <util/array_list.h>
#include <world/block/block.h>
#include <world/block/blocks.h>
#include <world/world.h>
#include <minecraft.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void entity_create(entity_t *entity, struct world_s *world) {
    //entity_t entity = { 0 };
    memset(entity, 0, sizeof(entity_t));
    entity->world = world;
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
    entity->item = NULL;
    entity->fire_resistance = 1;
    entity->is_first_update = 1;
    entity->air_supply = 300;

    entity->tick = entity_tick;
    entity->render = entity_render;
    entity->hurt = entity_hurt;
    entity->cause_fall_damage = entity_cause_fall_damage;
    entity->award_kill_score = entity_award_kill_score;
    entity->get_brightness = entity_get_brightness;
    entity->heal = entity_heal;
    entity->can_be_hit = entity_can_be_hit;
    entity->player_touch = entity_player_touch;
    entity->write_nbt = entity_write_nbt;
    entity->read_nbt = entity_read_nbt;

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
    if(!entity->world) return;
    world_t *world = (world_t *)entity->world;
    float x = world->spawn_x + 0.5;
    float y = world->spawn_y;
    float z = world->spawn_z + 0.5;

    for(; z > 0; z++) {
        entity_set_pos(entity, x, y, z);
        AABB_t *cubes = world_get_cubes((world_t *)entity->world, entity->bb);
        if(array_list_length(cubes) == 0) {
            array_list_free(cubes);
            break;
        }
        array_list_free(cubes);
    }

    entity->xd = entity->yd = entity->zd = 0;
    //entity->y_rot = world->spawn_rot;
    entity->x_rot = 0;
}

void entity_turn(entity_t *entity, float y_rot, float x_rot) {
    float curr_x_rot = entity->x_rot;
    float curr_y_rot = entity->y_rot;
    entity->y_rot = (float)((double)entity->y_rot + (double)x_rot * 0.15);
    entity->x_rot = (float)((double)entity->x_rot - (double)y_rot * 0.15);
    if(entity->x_rot < -90.0F) {
        entity->x_rot = -90.0F;
    }

    if(entity->x_rot > 90.0F) {
        entity->x_rot = 90.0F;
    }

    entity->x_roto = entity->x_roto + (entity->x_rot - curr_x_rot);
    entity->y_roto = entity->y_roto + (entity->y_rot - curr_y_rot);
}

void entity_interpolate_turn(entity_t *entity, float y_rot, float x_rot) {
    entity->y_rot = (float)((double)entity->y_rot + (double)y_rot * 0.15);
    entity->x_rot = (float)((double)entity->x_rot - (double)x_rot * 0.15);
    if(entity->x_rot < -90.0F) {
        entity->x_rot = -90.0F;
    }

    if(entity->x_rot > 90.0F) {
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
    if(entity_is_in_water(entity)) {
        if(!entity->in_water && !entity->is_first_update) {
            float velocity = sqrtf(entity->xd * entity->xd * 0.2 + entity->yd * entity->yd + entity->zd * entity->zd * 0.2) * 0.2;
            if(velocity > 1.0) {
                velocity = 1.0;
            }

            world_play_sound_at_entity(entity->world, entity, SOUND_RANDOM_SPLASH, velocity, 1.0 + (random_next_double(&entity->world->random) - random_next_double(&entity->world->random)) * 0.4);
            
            for(int i = 0; i < 1.0 + entity->bb_width * 20.0; i++) {
                float particle_x = (random_next_double(&entity->world->random) * 2.0 - 1.0) * entity->bb_width;
                float particle_z = (random_next_double(&entity->world->random) * 2.0 - 1.0) * entity->bb_width;
                world_spawn_particle(entity->world, PARTICLE_BUBBLE, entity->x + particle_x, floor_double(entity->bb.y0) + 1.0, entity->z + particle_z, entity->xd, entity->yd - random_next_double(&entity->world->random) * 0.2, entity->zd);
            }

            for(int i = 0; i < 1.0 + entity->bb_width * 20.0; i++) {
                float particle_x = (random_next_double(&entity->world->random) * 2.0 - 1.0) * entity->bb_width;
                float particle_z = (random_next_double(&entity->world->random) * 2.0 - 1.0) * entity->bb_width;
                world_spawn_particle(entity->world, PARTICLE_SPLASH, entity->x + particle_x, floor_double(entity->bb.y0) + 1.0, entity->z + particle_z, entity->xd, entity->yd, entity->zd);
            }
        }

        entity->fall_distance = 0;
        entity->in_water = 1;
        entity->fire = 0;
    }else {
        entity->in_water = 0;
    }

    if(entity->fire > 0) {
        if(entity->fire % 20 == 0) {
            entity_hurt(entity, NULL, 1);
        }
        entity->fire--;
    }

    if(entity_is_in_lava(entity)) {
        entity_hurt(entity, NULL, 10);
        entity->fire = 600;
    }

    entity->is_first_update = 0;
}

void entity_play_sound(entity_t *entity, uint8_t sound, float volume, float pitch) {
    world_play_sound_at_entity((world_t *)entity->world, entity, sound, volume, pitch);
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
    AABB_t *cubes = world_get_cubes((world_t *)entity->world, AABB_expand(entity->bb, x, y, z));

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
        AABB_t *cubes = world_get_cubes((world_t *)entity->world, AABB_expand(entity->bb, x, y, z));

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
        uint8_t block_id = world_get_block((world_t *)entity->world, floor_double(entity->x), floor_double(entity->y) - entity->height_offset - 0.2, floor_double(entity->z));
        if(entity->walk_dist > entity->next_step && block_id > blocks.air.id) {
            entity->next_step++;
            block_sound_t *sound = block_list[block_id].sound;
            if(sound->type != BLOCK_SOUND_NONE) {
                world_play_sound_at_entity(entity->world, entity, sound->base_type, sound->volume * 0.15, sound->pitch);
            }

            block_list[block_id].on_walked_upon(&block_list[block_id], entity->world, floor_double(entity->x), floor_double(entity->y - 0.2 - entity->height_offset), floor_double(entity->z));
        }
    }
    entity->y_slide_offset *= 0.4;
    uint8_t is_in_water = entity_is_in_water(entity);
    if(world_is_bounding_box_burning(entity->world, entity->bb)) {
        entity_hurt(entity, NULL, 1);
        if(!is_in_water) {
            entity->fire++;
            if(entity->fire == 0) {
                entity->fire = 300;
            }
        }
    }else if(entity->fire <= 0) {
        entity->fire = -entity->fire_resistance;
    }

    if(is_in_water && entity->fire > 0) {
        world_play_sound_at_entity(entity->world, entity, SOUND_RANDOM_FIZZ, 0.7, 1.6 + (random_next_double(&entity->world->random) - random_next_double(&entity->world->random)) * 0.4);
        entity->fire = -entity->fire_resistance;
    }
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
    speed = tsin(entity->y_rot * M_PI / 180.0);
    distance = tcos(entity->y_rot * M_PI / 180.0);
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
    entity->is_dead = 1;
}

uint8_t entity_is_free(entity_t *entity, float x, float y, float z) {
    AABB_t bb = AABB_move(entity->bb, x, y, z);
    AABB_t *cubes = world_get_cubes((world_t *)entity->world, bb);
    uint8_t free = array_list_length(cubes) > 0 ? 0 : !world_is_liquid_in_range((world_t *)entity->world, bb);
    array_list_free(cubes);
    return free;
}

uint8_t entity_is_underwater(entity_t *entity) {
    uint8_t block_id = world_get_block((world_t *)entity->world, floor_double(entity->x), floor_double(entity->y), floor_double(entity->z));
    return block_id != 0 ? block_list[block_id].material == &materials.water : 0;
}

uint8_t entity_is_in_water(entity_t *entity) {
    return world_is_material_in_box((world_t *)entity->world, AABB_grow(entity->bb, 0, -0.4, 0), &materials.water);
}

uint8_t entity_is_in_lava(entity_t *entity) {
    return world_is_material_in_box((world_t *)entity->world, AABB_grow(entity->bb, 0, -0.4, 0), &materials.lava);
}

uint8_t entity_on_ground(entity_t *entity) {
    return entity->on_ground;
}

float entity_get_brightness(entity_t *entity, float tick) {
    return world_get_brightness((world_t *)entity->world, floor_double(entity->x), floor_double(entity->y + (entity->height_offset / 2.0)), floor_double(entity->z));
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

char *entity_get_name(entity_t *entity) {
    switch(entity->type) {
        case ENTITY_ARROW: return "Arrow";
        case ENTITY_ITEM: return "Item";
        case ENTITY_PAINTING: return "Painting";
        case ENTITY_MOB: return "Mob";
        // monster
        case ENTITY_MOB_CREEPER: return "Creeper";
        case ENTITY_MOB_SKELETON: return "Skeleton";
        case ENTITY_MOB_SPIDER: return "Spider";
        case ENTITY_MOB_GIANT: return "Giant";
        case ENTITY_MOB_ZOMBIE: return "Zombie";
        case ENTITY_MOB_PIG: return "Pig";
        case ENTITY_MOB_SHEEP: return "Sheep";
        case ENTITY_PRIMED_TNT: return "PrimedTnt";
        default: return NULL;
    }
}

int entity_get_type(char *name) {
    if(!name) return -1;
    if(!strcmp(name, "Arrow")) return ENTITY_ARROW;
    if(!strcmp(name, "Item")) return ENTITY_ITEM;
    if(!strcmp(name, "Painting")) return ENTITY_PAINTING;
    if(!strcmp(name, "Mob")) return ENTITY_MOB;
    if(!strcmp(name, "Monster")) return ENTITY_MOB;
    if(!strcmp(name, "Creeper")) return ENTITY_MOB_CREEPER;
    if(!strcmp(name, "Skeleton")) return ENTITY_MOB_SKELETON;
    if(!strcmp(name, "Spider")) return ENTITY_MOB_SPIDER;
    if(!strcmp(name, "Giant")) return ENTITY_MOB_GIANT;
    if(!strcmp(name, "Zombie")) return ENTITY_MOB_ZOMBIE;
    if(!strcmp(name, "Pig")) return ENTITY_MOB_PIG;
    if(!strcmp(name, "Sheep")) return ENTITY_MOB_SHEEP;
    if(!strcmp(name, "PrimedTnt")) return ENTITY_PRIMED_TNT;
    return -1;
}

void entity_get_constructor(entity_t *entity, world_t *world, uint8_t type) {
    switch(type) {
        case ENTITY_ARROW: entity_arrow_create(entity, world, NULL, 0, 0, 0, 0, 0, 0); break;
        case ENTITY_ITEM: entity_item_create(entity, world, 0, 0, 0, item_stack_create(0, 0, 0)); break;
        case ENTITY_PAINTING: entity_painting_create(entity, world, 0, 0, 0, 0); break;
        case ENTITY_MOB: mob_create(entity, world); break;
        // monster
        case ENTITY_MOB_CREEPER: mob_creeper_create(entity, world, 0, 0, 0); break;
        case ENTITY_MOB_SKELETON: mob_skeleton_create(entity, world, 0, 0, 0); break;
        case ENTITY_MOB_SPIDER: mob_spider_create(entity, world, 0, 0, 0); break;
        case ENTITY_MOB_GIANT: mob_giant_create(entity, world, 0, 0, 0); break;
        case ENTITY_MOB_ZOMBIE: mob_zombie_create(entity, world, 0, 0, 0); break;
        case ENTITY_MOB_PIG: mob_pig_create(entity, world, 0, 0, 0); break;
        case ENTITY_MOB_SHEEP: mob_sheep_create(entity, world, 0, 0, 0); break;
        case ENTITY_PRIMED_TNT: entity_primed_tnt_create(entity, world, 0, 0, 0); break;
        default: break;
    }
}

uint8_t entity_add_id(entity_t *entity, nbt_base_t *nbt) {
    char *name = entity_get_name(entity);
    if(!entity->is_dead && name != NULL) {
        nbt_tag_compound_set_string(nbt, "id", name);
        entity->write_nbt(entity, nbt);
        return 1;
    }
    return 0;
}

void entity_create_from_nbt(entity_t *entity, nbt_base_t *nbt, world_t *world) {
    char *name = nbt_tag_compound_get_string(nbt, "id");
    int type = entity_get_type(name);
    entity_get_constructor(entity, world, type);
    if(entity) {
        entity->read_nbt(entity, nbt);
    }else {
        printf("Skipping Entity with id %s\n", name);
    }
}

void entity_read_nbt(entity_t *entity, nbt_base_t *nbt) {
    nbt_base_t pos_list = nbt_tag_compound_get_tag_list(nbt, "Pos");
    nbt_base_t motion_list = nbt_tag_compound_get_tag_list(nbt, "Motion");
    nbt_base_t rotation_list = nbt_tag_compound_get_tag_list(nbt, "Rotation");

    entity->x = nbt_tag_list_get_tag(&pos_list, 0)->double_value;
    entity->last_tick_x = entity->x;
    entity->xo = entity->last_tick_x;
    entity->y = nbt_tag_list_get_tag(&pos_list, 1)->double_value;
    entity->last_tick_y = entity->y;
    entity->yo = entity->last_tick_y;
    entity->z = nbt_tag_list_get_tag(&pos_list, 2)->double_value;
    entity->last_tick_z = entity->z;
    entity->zo = entity->last_tick_z;

    entity->xd = nbt_tag_list_get_tag(&motion_list, 0)->double_value;
    entity->yd = nbt_tag_list_get_tag(&motion_list, 1)->double_value;
    entity->zd = nbt_tag_list_get_tag(&motion_list, 2)->double_value;

    entity->x_rot = nbt_tag_list_get_tag(&rotation_list, 0)->float_value;
    entity->y_rot = nbt_tag_list_get_tag(&rotation_list, 1)->float_value;

    entity->fall_distance = nbt_tag_compound_get_float(nbt, "FallDistance");
    entity->fire = nbt_tag_compound_get_short(nbt, "Fire");
    entity->air_supply = nbt_tag_compound_get_short(nbt, "Air");
    entity_set_pos(entity, entity->x, entity->y, entity->z);
}

void entity_write_nbt(entity_t *entity, nbt_base_t *nbt) {
    nbt_base_t pos_list = nbt_tag_list_create();
    nbt_base_t motion_list = nbt_tag_list_create();
    nbt_base_t rotation_list = nbt_tag_list_create();

    nbt_base_t pos_x = nbt_tag_double_create(entity->x);
    nbt_base_t pos_y = nbt_tag_double_create(entity->y);
    nbt_base_t pos_z = nbt_tag_double_create(entity->z);
    nbt_tag_list_set_tag(&pos_list, &pos_x);
    nbt_tag_list_set_tag(&pos_list, &pos_y);
    nbt_tag_list_set_tag(&pos_list, &pos_z);
    nbt_tag_compound_set_tag(nbt, "Pos", &pos_list);

    nbt_base_t motion_x = nbt_tag_double_create(entity->xd);
    nbt_base_t motion_y = nbt_tag_double_create(entity->yd);
    nbt_base_t motion_z = nbt_tag_double_create(entity->zd);
    nbt_tag_list_set_tag(&motion_list, &motion_x);
    nbt_tag_list_set_tag(&motion_list, &motion_y);
    nbt_tag_list_set_tag(&motion_list, &motion_z);
    nbt_tag_compound_set_tag(nbt, "Motion", &motion_list);

    nbt_base_t rotation_x = nbt_tag_float_create(entity->x_rot);
    nbt_base_t rotation_y = nbt_tag_float_create(entity->y_rot);
    nbt_tag_list_set_tag(&rotation_list, &rotation_x);
    nbt_tag_list_set_tag(&rotation_list, &rotation_y);
    nbt_tag_compound_set_tag(nbt, "Rotation", &rotation_list);

    nbt_tag_compound_set_float(nbt, "FallDistance", entity->fall_distance);
    nbt_tag_compound_set_short(nbt, "Fire", entity->fire);
    nbt_tag_compound_set_short(nbt, "Air", entity->air_supply);
}