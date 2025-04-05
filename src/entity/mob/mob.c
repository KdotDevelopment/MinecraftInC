#include <entity/mob/mob.h>
#include <level/level.h>
#include <model/models.h>
#include <model/model.h>
#include <model/model_humanoid.h>
#include <minecraft.h>

#include <math.h>
#include <stddef.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

void mob_create(mob_t *mob, struct level_s *level) {
    //mob_t mob = { 0 };
    entity_create(&mob->entity, level);

    mob->invulnerable_duration = INVULNERABLE_DURATION;
    mob->y_body_rot = 0.0F;
    mob->y_body_roto = 0.0F;
    mob->tick_count = 0;
    mob->has_hair = 1;
    mob->allow_alpha = 1;
    mob->bob_strength = 1.0F;
    mob->death_score = 0;
    mob->render_offset = 0;
    mob->invulnerable_time = 0;
    mob->air_supply = AIR_SUPPLY;
    mob->hurt_dir = 0;
    mob->death_time = 0;
    mob->attack_time = 0;
    mob->dead = 0;
    mob->texture_name = "char.png";
    mob->anim_step = 0;
    mob->anim_stepo = 0;
    mob->hurt_duration = 1;

    mob->health = 20;
    mob->time_offs = random_uniform() * M_PI * 12398.0;
    mob->rot = random_uniform() * M_PI * 2;
    mob->speed = 1;
    mob->ai = ai_basic_create(level, mob);
    mob->foot_size = 0.5;

    mob->hurt = mob_hurt;
    mob->render = mob_render;
    mob->tick = mob_tick;
    mob->cause_fall_damage = mob_cause_fall_damage;
    mob->render_model = mob_render_model;
    mob->die = mob_die;
    mob->ai_step = mob_ai_step;
    mob->heal = mob_heal;
    mob->can_be_hit = mob_can_be_hit;

    //return mob;
}

void mob_tick(entity_t *entity) {
    mob_t *mob = (mob_t *)entity;
    entity_tick(entity);
    mob->o_tilt = mob->tilt;
    if(mob->attack_time > 0) {
        mob->attack_time--;
    }

    if(mob->hurt_time > 0) {
        mob->hurt_time--;
    }

    if(mob->invulnerable_time > 0) {
        mob->invulnerable_time--;
    }

    if(mob->health <= 0) {
        mob->death_time++;
        if(mob->death_time > 20) {
            if(mob->ai.before_removal != NULL) {
                mob->ai.before_removal((struct ai_s *)&mob->ai);
            }
            entity_remove(&mob->entity);
        }
    }

    if(entity_is_underwater(&mob->entity)) {
        if(mob->air_supply > 0) {
            mob->air_supply--;
        } else {
            mob->hurt(&mob->entity, NULL, 2);
        }
    } else {
        mob->air_supply = AIR_SUPPLY;
    }

    if(entity_is_in_lava(&mob->entity)) {
        mob->hurt(&mob->entity, NULL, 10);
    }

    mob->anim_stepo = mob->anim_step;
    mob->y_body_roto = mob->y_body_rot;
    mob->y_roto = mob->y_rot;
    mob->x_roto = mob->x_rot;
    mob->tick_count++;
    mob->ai_step(mob);
    float x_diff = mob->x - mob->xo;
    float z_diff = mob->z - mob->zo;
    float dist = sqrt(x_diff * x_diff + z_diff * z_diff);
    float y_body_rot = mob->y_body_rot;
    float accumulator = 0;
    mob->o_run = mob->run;

    float toggle = 0;
    if(!(dist <= 0.05)) {
        toggle = 1;
        accumulator = dist * 3;
        y_body_rot = atan2(z_diff, x_diff) * 180 / M_PI - 90;
    }

    if(mob->on_ground) {
        toggle = 0;
    }

    mob->run = mob->run + (toggle - mob->run) * 0.3;
    x_diff = y_body_rot - mob->y_body_rot;

    while(x_diff < -180) {
        x_diff += 360;
    }

    while(x_diff >= 180) {
        x_diff -= 360;
    }

    mob->y_body_rot += x_diff * 0.1;
    x_diff = mob->y_rot - mob->y_body_rot;

    while(x_diff < -180) {
        x_diff += 360;
    }

    while(x_diff >= 180) {
        x_diff -= 360;
    }

    uint8_t out_of_bounds = x_diff < -90 || x_diff >= 90;
    if(x_diff < -75) {
        x_diff = -75;
    }

    if(x_diff >= 75) {
        x_diff = 75;
    }

    mob->y_body_rot = mob->y_rot - x_diff;
    mob->y_body_rot += x_diff * 0.1;

    if(out_of_bounds) {
        accumulator = -accumulator;
    }

    while(mob->y_rot - mob->y_body_rot < -180) {
        mob->y_body_rot -= 360;
    }

    while(mob->y_rot - mob->y_body_rot >= 180) {
        mob->y_body_rot += 360;
    }

    while(mob->y_body_rot - mob->y_body_roto < -180) {
        mob->y_body_roto -= 360;
    }

    while(mob->y_body_rot - mob->y_body_roto >= 180) {
        mob->y_body_roto += 360;
    }

    while(mob->x_rot - mob->x_roto < -180) {
        mob->x_roto -= 360;
    }

    while(mob->x_rot - mob->x_roto >= 180) {
        mob->x_roto += 360;
    }

    mob->anim_step += accumulator;
}

void mob_ai_step(struct mob_s *mob) {
    if(&mob->ai != NULL) {
        mob->ai.tick((struct ai_s *)&mob->ai);
    }
}

void mob_render(entity_t *entity, textures_t *textures, float delta) {
    mob_t *mob = (mob_t *)entity;
    if(entity->type == ENTITY_MOB_PLAYER) return;
    if(mob->model_type == -1) return;
    float attack_time = mob->attack_time - delta;
    if(attack_time < 0) {
        attack_time = 0;
    }

    while(mob->y_body_rot - mob->y_body_roto < -180) {
        mob->y_body_roto -= 360;
    }

    while(mob->y_body_rot - mob->y_body_roto >= 180) {
        mob->y_body_roto += 360;
    }

    while(mob->x_rot - mob->x_roto < -180) {
        mob->x_roto -= 360;
    }

    while(mob->x_rot - mob->x_roto >= 180) {
        mob->x_roto += 360;
    }

    while(mob->y_rot - mob->y_body_rot < -180) {
        mob->y_body_rot -= 360;
    }

    while(mob->y_rot - mob->y_body_rot >= 180) {
        mob->y_body_rot += 360;
    }

    float delta_y_body_rot = mob->y_body_roto + (mob->y_body_rot - mob->y_body_roto) * delta;
    float delta_run = mob->o_run + (mob->run - mob->o_run) * delta;
    float delta_y_rot = mob->y_roto + (mob->y_rot - mob->y_roto) * delta;
    float delta_x_rot = mob->x_roto + (mob->x_rot - mob->x_roto) * delta;

    delta_y_rot -= delta_y_body_rot;

    glPushMatrix();

    float delta_anim_step = mob->anim_stepo + (mob->anim_step - mob->anim_stepo) * delta;
    float brightness = entity->get_brightness(&mob->entity, delta);
    glColor3f(brightness, brightness, brightness);

    float bob = abs(cos(delta_anim_step * 0.6662)) * 5.0 * delta_run * mob->bob_strength - 23;

    glTranslatef(mob->xo + (mob->x - mob->xo) * delta, 
                 mob->yo + (mob->y - mob->yo) * delta - 1.62 + mob->render_offset, 
                 mob->zo + (mob->z - mob->zo) * delta);

    float hurt_time = mob->hurt_time - delta;

    if(hurt_time > 0 || mob->health <= 0) {
        if(hurt_time < 0) {
            hurt_time = 0;
        }else {
            float hurt_dur = mob->hurt_time / mob->hurt_duration;
            hurt_time = tsin(hurt_dur * hurt_dur * hurt_dur * hurt_dur * M_PI) * 14;
        }

        float death_time = (mob->death_time + delta) / 20;
        if(mob->health <= 0) {
            hurt_time += death_time * death_time * 800;
            if(hurt_time > 90) {
                hurt_time = 90;
            }
        }

        death_time = mob->hurt_dir;
        glRotatef(180 - delta_y_body_rot + mob->rot_offs, 0, 1, 0);
        glScalef(1, 1, 1);
        glRotatef(-death_time, 0, 1, 0);
        glRotatef(-hurt_time, 0, 0, 1);
        glRotatef(death_time, 0, 1, 0);
        glRotatef(-(180 - delta_y_body_rot + mob->rot_offs), 0, 1, 0);
    }

    glTranslatef(0, -bob * 0.0625, 0);
    glScalef(1, -1, 1);
    glRotatef(180 - delta_y_body_rot + mob->rot_offs, 0, 1, 0);
    if(!mob->allow_alpha) {
        glDisable(GL_ALPHA_TEST);
    }else {
        glDisable(GL_CULL_FACE);
    }

    glScalef(-1, 1, 1);
    //model_t *model = models_get(&mob->level->minecraft->models, mob->model_type);
    entity->model->attack_time = attack_time / 5;
    glBindTexture(GL_TEXTURE_2D, textures_load(textures, mob->texture_name));
    mob->render_model(mob, delta_anim_step, delta, delta_run, delta_y_rot, delta_x_rot, 0.0625);
    if(mob->invulnerable_time > mob->invulnerable_duration - 10) {
        glColor4f(1, 1, 1, 0.75);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glBindTexture(GL_TEXTURE_2D, textures_load(textures, mob->texture_name));
        mob->render_model(mob, delta_anim_step, delta, delta_run, delta_y_rot, delta_x_rot, 0.0625);
        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    glEnable(GL_ALPHA_TEST);
    if(mob->allow_alpha) {
        glEnable(GL_CULL_FACE);
    }

    glColor4f(1, 1, 1, 1);
    glPopMatrix();
}

void mob_hurt(entity_t *entity, entity_t *entity_causer, int damage) {
    mob_t *mob = (mob_t *)entity;
    mob_t *causer = (mob_t *)entity_causer;
    level_t *level = (level_t *)entity->level;
    if(level->creative_mode) return;
    if(mob->health <= 0) return;
    mob->ai.hurt(&mob->ai, entity_causer, damage);
    if(mob->invulnerable_time > mob->invulnerable_duration / 2) {
        if(mob->last_health - damage >= mob->health) return;
        mob->health = mob->last_health - damage;
    }else {
        mob->last_health = mob->health;
        mob->invulnerable_time = mob->invulnerable_duration;
        mob->health -= damage;
        mob->hurt_time = mob->hurt_duration = 10;
    }

    mob->hurt_dir = 0;
    if(causer) {
        float x_diff = causer->x - mob->x;
        float z_diff = causer->z - mob->z;
        mob->hurt_dir = atan2(z_diff, x_diff) * 180 / M_PI - mob->y_rot;
        mob_knockback(mob, causer, damage, x_diff, z_diff);
    }else {
        mob->hurt_dir = (rand() % 2) * 180;
    }

    if(mob->health <= 0) {
        mob->die(mob, causer);
    }
}

void mob_knockback(mob_t *mob, mob_t *causer, int damage, float x_diff, float z_diff) {
    float dist = sqrt(x_diff * x_diff + z_diff * z_diff);
    float knockback = 0.4;
    mob->xd /= 2;
    mob->yd /= 2;
    mob->zd /= 2;
    mob->xd -= x_diff / dist * knockback;
    mob->yd += 0.4;
    mob->zd -= z_diff / dist * knockback;
    if(mob->yd > 0.4) {
        mob->yd = 0.4;
    }
}

void mob_die(struct mob_s *mob, mob_t *causer) {
    level_t *level = (level_t *)mob->level;
    if(level->creative_mode) return;
    if(mob->death_score > 0 && causer) {
        mob->award_kill_score(&mob->entity, (entity_t *)&causer, mob->death_score);
    }

    mob->dead = 1;
}

void mob_cause_fall_damage(entity_t *entity, float distance) {
    mob_t *mob = (mob_t *)entity;
    level_t *level = (level_t *)entity->level;
    entity_cause_fall_damage(&mob->entity, distance);
    if(level->creative_mode) return;
    int damage = (int)ceil(distance - 3);
    if(damage > 0) {
        mob->hurt(&mob->entity, NULL, damage);
    }
}

void mob_travel(mob_t *mob, float x, float z) {
    if(entity_is_in_water(&mob->entity)) {
        float y = mob->y;
        entity_move_relative(&mob->entity, x, z, 0.02);
        entity_move(&mob->entity, mob->xd, mob->yd, mob->zd);
        mob->xd *= 0.8;
        mob->yd *= 0.8;
        mob->zd *= 0.8;
        mob->yd -= 0.02;
        if(mob->horizontal_collision && entity_is_free(&mob->entity, mob->xd, mob->yd + 0.6 - mob->y + y, mob->zd)) {
            mob->yd = 0.3;
        }
    }else if(entity_is_in_lava(&mob->entity)) {
        float y = mob->y;
        entity_move_relative(&mob->entity, x, z, 0.02);
        entity_move(&mob->entity, mob->xd, mob->yd, mob->zd);
        mob->xd *= 0.5;
        mob->yd *= 0.5;
        mob->zd *= 0.5;
        mob->yd -= 0.02;
        if(mob->horizontal_collision && entity_is_free(&mob->entity, mob->xd, mob->yd + 0.6 - mob->y + y, mob->zd)) {
            mob->yd = 0.3;
        }
    }else {
        entity_move_relative(&mob->entity, x, z, entity_on_ground(&mob->entity) ? 0.1 : 0.02);
        entity_move(&mob->entity, mob->xd, mob->yd, mob->zd);
        mob->xd *= 0.91;
        mob->yd *= 0.98;
        mob->zd *= 0.91;
        mob->yd -= 0.08;
        if(entity_on_ground(&mob->entity)) {
            mob->xd *= 0.6;
            mob->zd *= 0.6;
        }
    }
}

void mob_heal(struct entity_s *entity, int amount) {
    mob_t *mob = (mob_t *)entity;
    if(mob->health <= 0) return;
    mob->health += amount;
    if(mob->health > 20) mob->health = 20;
    mob->invulnerable_time = mob->invulnerable_duration / 2;
}

void mob_render_model(mob_t *mob, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    mob->model->render(mob->model, time, r, bob, y_rot, x_rot, scale);
}

uint8_t mob_can_be_hit(entity_t *entity) {
    return !entity->removed;
}