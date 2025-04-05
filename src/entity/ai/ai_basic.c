#include <entity/ai/ai_basic.h>
#include <entity/ai/ai.h>
#include <level/level.h>
#include <entity/mob/mob.h>

#include <stdlib.h>

ai_t ai_basic_create(struct level_s *level, struct mob_s *mob) {
    level_t *real_level = (level_t *)level;
    ai_t ai = ai_create();
    ai.run_speed = 0.7;
    ai.mob = mob;
    ai.level = level;
    ai.random = &real_level->random;
    ai.tick = ai_basic_tick;
    ai.hurt = ai_basic_hurt;
    ai.update = ai_basic_update;
    ai.jump = ai_basic_jump;

    return ai;
}

void ai_basic_tick(struct ai_s *ai) {
    level_t *level = (level_t *)ai->level;
    mob_t *mob = (mob_t *)ai->mob;
    ai->no_action_time++;
    entity_t *entity;
    if(ai->no_action_time > 600 && random_next_int_range(ai->random, 0, 800) == 0 && (entity = (entity_t *)&level->player->mob) != NULL) {
        float x_diff = entity->x - mob->x;
        float y_diff = entity->y - mob->y;
        float z_diff = entity->z - mob->z;

        if(x_diff * x_diff + y_diff * y_diff + z_diff * z_diff < 1024.0) {
            ai->no_action_time = 0;
        }else {
            entity_remove(&mob->entity);
        }
    }

    if(ai->attack_delay > 0) {
        ai->attack_delay--;
    }

    if(mob->health <= 0) {
        ai->jumping = 0;
        ai->xa = 0;
        ai->za = 0;
        ai->y_rota = 0;
    }else {
        ai->update(ai);
    }

    uint8_t water = entity_is_in_water(&mob->entity);
    uint8_t lava = entity_is_in_lava(&mob->entity);
    if(ai->jumping) {
        if(water) {
            mob->yd += 0.04;
        }else if(lava) {
            mob->yd += 0.04;
        }else if(entity_on_ground(&mob->entity)) {
            ai_basic_jump(ai);
        }
    }

    ai->xa *= 0.98;
    ai->za *= 0.98;
    ai->y_rota *= 0.9;
    mob_travel(mob, ai->xa, ai->za);
    //entity grid, blockmap etc (collision detection)
    //hint: blockmap is actually just entity_t **entities which is already in level
}

void ai_basic_jump(ai_t *ai) {
    mob_t *mob = (mob_t *)ai->mob;
    mob->yd = 0.42;
}

void ai_basic_update(struct ai_s *ai) {
    mob_t *mob = (mob_t *)ai->mob;
    if(random_next_uniform(ai->random) < 0.07) {
        ai->xa = (random_next_uniform(ai->random) - 0.5) * ai->run_speed;
        ai->za = random_next_uniform(ai->random) * ai->run_speed;
    }

    ai->jumping = random_next_uniform(ai->random) < 0.01;
    if(random_next_uniform(ai->random) < 0.04) {
        ai->y_rota = (random_next_uniform(ai->random) - 0.5) * 60.0;
    }

    ai->mob->y_rot += ai->y_rota;
    ai->mob->x_rot += ai->default_look_angle;
    if(ai->attack_target != NULL) {
        ai->za = ai->run_speed;
        ai->jumping = random_next_uniform(ai->random) < 0.04;
    }

    uint8_t water = entity_is_in_water(&mob->entity);
    uint8_t lava = entity_is_in_lava(&mob->entity);
    if(water || lava) {
        ai->jumping = random_next_uniform(ai->random) < 0.8;
    }
}

void ai_basic_hurt(struct ai_s *ai, struct entity_s *entity, int damage) {
    ai_hurt(ai, entity, damage);
    ai->no_action_time = 0;
}