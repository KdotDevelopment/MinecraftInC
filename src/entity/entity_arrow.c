#include <entity/entity_arrow.h>
#include <level/level.h>
#include <renderer/tesselator.h>

#include <util/sin_table.h>
#include <util/array_list.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

void entity_arrow_create(entity_t *entity, struct level_s *level, entity_t *source_entity, float x, float y, float z, float xd, float yd, float zd) {
    entity_create(entity, level);
    entity->bb_width = 0.3;
    entity->bb_height = 0.5;
    entity->type = ENTITY_ARROW;
    entity->damage = 3;
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
    x -= s * 0.2;
    z += c * 0.2;
    entity->xd = c * xd * zd;
    entity->yd = yd * zd;
    entity->zd = s * xd * zd;
    entity_set_pos(entity, x, y, z);

    float magnitude = sqrtf(entity->xd * entity->xd + entity->zd * entity->zd);
    entity->y_rot = (float)(atan2(entity->xd, entity->zd) * 180.0 / M_PI);
    entity->x_rot = (float)(atan2(entity->yd, magnitude) * 180.0 / M_PI);

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
        if(entity->arrow_type  == 0) {
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
        if(level_get_cubes((level_t *)entity->level, bb) > 0) {
            entity->has_hit = 1;
        }

        entity_t ***entities = entity_map_get_entities(entity->entity_map, entity, bb.x0, bb.y0, bb.z0, bb.x1, bb.y1, bb.z1);

        for(int j = 0; j < array_list_length(*entities); j++) {
            entity_t *target = *(entity_t **)array_list_get(*entities, j);
            if(target->is_shootable && (target != entity->owner || entity->time > 5)) {
                target->hurt(target, entity, entity->damage);
                entity->has_hit = 1;
                entity_remove(entity);
                return;
            }
        }

        if(!entity->has_hit) {
            entity->bb = AABB_move(entity->bb, nxd, nyd, nzd);
            entity->x += nxd;
            entity->y += nyd;
            entity->z += nzd;
            entity_map_moved(entity->entity_map, entity);
        }
    }

    if(entity->has_hit) {
        entity->xd = 0;
        entity->yd = 0;
        entity->zd = 0;
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
    glBindTexture(GL_TEXTURE_2D, textures_load(textures, "item/arrow.png"));
    float brightness = level_get_brightness((level_t *)entity->level, entity->x, entity->y, entity->z);
    glPushMatrix();
    glColor4f(brightness, brightness, brightness, 1);
    glTranslatef(entity->xo + (entity->x - entity->xo) * delta, 
                 entity->yo + (entity->y - entity->yo) * delta, 
                 entity->zo + (entity->z - entity->zo) * delta);
    glRotatef(entity->y_roto + (entity->y_rot - entity->y_roto) * delta - 90.0, 0, 1, 0);
    glRotatef(entity->x_roto + (entity->x_rot - entity->x_roto) * delta, 0, 0, 1);
    glRotatef(45, 1, 0, 0);
    delta = 0.5;
    float fins_v1 = (0 + entity->arrow_type * 10) / 32.0;
    float stem_v1 = (5 + entity->arrow_type * 10) / 32.0;
    float fins_v2 = (5 + entity->arrow_type * 10) / 32.0;
    float stem_v2 = (10 + entity->arrow_type * 10) / 32.0;
    float u = 0.15625;
    float b = 0.05625;
    glScalef(b, b, b);
    glNormal3f(b, 0, 0);
    tesselator_begin();
    tesselator_vertex_uv(-7, -2, -2, 0, stem_v1);
    tesselator_vertex_uv(-7, -2, 2, u, stem_v1);
    tesselator_vertex_uv(-7, 2, 2, u, stem_v2);
    tesselator_vertex_uv(-7, 2, -2, 0, stem_v2);
    tesselator_end();
    glNormal3f(-b, 0, 0);
    tesselator_begin();
    tesselator_vertex_uv(-7, 2, -2, 0, stem_v1);
    tesselator_vertex_uv(-7, 2, 2, u, stem_v1);
    tesselator_vertex_uv(-7, -2, 2, u, stem_v2);
    tesselator_vertex_uv(-7, -2, -2, 0, stem_v2);
    tesselator_end();
    
    for(int i = 0; i < 4; i++) {
        glRotatef(90, 1, 0, 0);
        glNormal3f(0, -b, 0);
        tesselator_begin(); //not in original? remove if not working
        tesselator_vertex_uv(-8, -2, 0, 0, fins_v1);
        tesselator_vertex_uv(8, -2, 0, delta, fins_v1);
        tesselator_vertex_uv(8, 2, 0, delta, fins_v2);
        tesselator_vertex_uv(-8, 2, 0, 0, fins_v2);
        tesselator_end();
    }

    glColor4f(1, 1, 1, 1);
    glPopMatrix();
}

void entity_arrow_award_kill_score(struct entity_s *entity, struct entity_s *causer, int score) {
    entity_award_kill_score(entity->owner, causer, score);
}

void entity_arrow_player_touch(struct entity_s *entity, entity_t *player) {
    //taking item animation
}

entity_t *entity_arrow_get_owner(entity_t *entity) {
    return entity->owner;
}