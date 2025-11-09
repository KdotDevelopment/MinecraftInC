#include <entity/entity_arrow.h>
#include <world/world.h>
#include <renderer/tesselator.h>

#include <util/math_helper.h>
#include <util/array_list.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

void entity_arrow_create(entity_t *entity, struct world_s *world, entity_t *source_entity, float x, float y, float z, float xd, float yd, float zd) {
    entity_create(entity, world);
    entity->bb_width = 0.3;
    entity->bb_height = 0.5;
    entity->type = ENTITY_ARROW;
    entity->damage = 3;
    entity->owner = source_entity;
    if(source_entity->type == ENTITY_MOB_PLAYER) {
        entity->damage = 7;
    }else {
        entity->arrow_type = 1;
    }

    entity->height_offset = 0.25;
    float c = tcos(-xd * (M_PI / 180.0) - M_PI);
    float s = tsin(-xd * (M_PI / 180.0) - M_PI);
    xd = tcos(-yd * (M_PI / 180.0));
    yd = tsin(-yd * (M_PI / 180.0));
    entity->slide = 0;
    entity->gravity = 1.0 / zd;
    entity->xo -= c * 0.2;
    entity->zo += s * 0.2;
    x -= c * 0.2;
    z += s * 0.2;
    entity->xd = s * xd * zd;
    entity->yd = yd * zd;
    entity->zd = c * xd * zd;
    entity_set_pos(entity, x, y, z);

    float magnitude = sqrtf(entity->xd * entity->xd + entity->zd * entity->zd);
    entity->y_rot = (float)(atan2(entity->xd, entity->zd) * 180.0 / M_PI);
    entity->x_rot = (float)(atan2(entity->yd, magnitude) * 180.0 / M_PI);
    entity->y_roto = entity->y_rot;
    entity->x_roto = entity->x_rot;

    entity->make_step_sound = 0;

    entity->tick = entity_arrow_tick;
    entity->render = entity_arrow_render;
    entity->player_touch = entity_arrow_player_touch;
}

void entity_arrow_tick(struct entity_s *entity) {
    entity->time++;
    entity->x_roto = entity->x_rot;
    entity->y_roto = entity->y_rot;
    entity->xo = entity->x;
    entity->yo = entity->y;
    entity->zo = entity->z;
    if(entity->has_hit) {
        entity->age++;
        if(entity->arrow_type == 0) {
            if(entity->age >= 300 && random_uniform() < 0.01) {
                entity_remove(entity);
            }
        }else if(entity->age >= 20) {
            entity_remove(entity);
        }
        return;
    }

    entity->xd *= 0.998;
    entity->yd *= 0.998;
    entity->zd *= 0.998;
    entity->yd = entity->yd - 0.02 * entity->gravity;
    int magnitude = sqrtf(entity->xd * entity->xd + entity->yd * entity->yd + entity->zd * entity->zd) / 0.2 + 1.0;
    float nxd = entity->xd / magnitude;
    float nyd = entity->yd / magnitude;
    float nzd = entity->zd / magnitude;
    
    for(int i = 0; i < magnitude; i++) {
        AABB_t bb = AABB_expand(entity->bb, nxd, nyd, nzd);
        AABB_t *cubes = world_get_cubes((world_t *)entity->world, bb);
        if(array_list_length(cubes) > 0) {
            entity->has_hit = 1;
        }
        array_list_free(cubes);

        /*entity_t ***entities = entity_map_get_entities(entity->entity_map, entity, bb.x0, bb.y0, bb.z0, bb.x1, bb.y1, bb.z1);

        for(int j = 0; j < array_list_length(*entities); j++) {
            entity_t *target = *(entity_t **)array_list_get(*entities, j);
            if(target->is_shootable && (target != entity->owner || entity->time > 5)) {
                target->hurt(target, entity, entity->damage);
                entity->has_hit = 1;
                entity_remove(entity);
                return;
            }
        }*/

        if(!entity->has_hit) {
            entity->bb = AABB_move(entity->bb, nxd, nyd, nzd);
            entity->x += nxd;
            entity->y += nyd;
            entity->z += nzd;
            //entity_map_moved(entity->entity_map, entity);
        }
    }

    if(entity->has_hit) {
        entity->xd = 0;
        entity->yd = 0;
        entity->zd = 0;
        world_play_sound_at_entity(entity->world, entity, SOUND_RANDOM_DRR, 1.0, 1.2 / (random_uniform() * 0.2 + 0.9));
    }

    if(!entity->has_hit) {
        entity->x_rot = (float)(atan2(entity->yd, sqrtf(entity->xd * entity->xd + entity->zd * entity->zd)) * 180.0 / M_PI);
        entity->y_rot = (float)(atan2(entity->xd, entity->zd) * 180.0 / M_PI);
        while(entity->x_rot - entity->x_roto < -180) {
            entity->x_roto -= 360;
        }
        while(entity->x_rot - entity->x_roto >= 180) {
            entity->x_roto += 360;
        }
        while(entity->y_rot - entity->y_roto < -180) {
            entity->y_roto -= 360;
        }
        while(entity->y_rot - entity->y_roto >= 180) {
            entity->y_roto += 360;
        }
    }
}

void entity_arrow_render(struct entity_s *entity, textures_t *textures, float delta) {
    
}

void entity_arrow_award_kill_score(struct entity_s *entity, struct entity_s *causer, int score) {
    entity_award_kill_score(entity->owner, causer, score);
}

void entity_arrow_player_touch(struct entity_s *entity, entity_t *player) {
    /*player_t *real_player = (player_t *)player;
    if(entity->has_hit && entity->owner == player && real_player->arrows < 99) {
        entity_t *anim = malloc(sizeof(entity_t));
        entity_take_anim_create(anim, entity->world, entity, real_player);
        world_spawn_entity(entity->world, anim);
        entity_remove(entity);
        real_player->arrows++;
    }*/
}

entity_t *entity_arrow_get_owner(entity_t *entity) {
    return entity->owner;
}