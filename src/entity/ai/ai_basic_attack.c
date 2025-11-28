#include <entity/ai/ai_basic_attack.h>

#include <entity/mob/mob.h>
#include <world/world.h>

#include <stddef.h>

ai_t ai_basic_attack_create(struct world_s *world, struct mob_s *mob) {
    ai_t ai = ai_basic_create(world, mob);
    ai.update = ai_basic_attack_update;
    ai.hurt = ai_basic_attack_hurt;
    ai.attack = ai_basic_attack_attack;

    return ai;
}

uint8_t ai_basic_attack_attack(struct ai_s *ai) {
    if(world_clip(ai->world, (vec3_t){ ai->mob->entity->x, ai->mob->entity->y, ai->mob->entity->z }, (vec3_t){ ai->attack_target->x, ai->attack_target->y, ai->attack_target->z }).null == 0) {
        return 0;
    }
    ai->mob->attack_time = 5;
    ai->attack_delay = random_next_int_range(ai->random, 21) + 10;
    int damage = (random_next_double(ai->random) * random_next_double(ai->random)) / 2.0 * ai->damage + 1;
    entity_hurt(ai->attack_target, (entity_t *)&ai->mob->entity, damage);
    ai->no_action_time = 0;
    return 1;
}

void ai_basic_attack_update(struct ai_s *ai) {
    ai_basic_update(ai);
    if(ai->mob->health > 0) {
        entity_t *entity = ai->world->player;
        float distance = 16.0;
        if(ai->attack_target != NULL && ai->attack_target->removed) {
            ai->attack_target = NULL;
        }

        if(entity != NULL && ai->attack_target == NULL) {
            float x_diff = entity->x - ai->mob->entity->x;
            float y_diff = entity->y - ai->mob->entity->y;
            float z_diff = entity->z - ai->mob->entity->z;

            if(x_diff * x_diff + y_diff * y_diff + z_diff * z_diff < distance * distance) {
                ai->attack_target = entity;
            }
        }

        if(ai->attack_target != NULL) {
            float x_diff = ai->attack_target->x - ai->mob->entity->x;
            float y_diff = ai->attack_target->y - ai->mob->entity->y;
            float z_diff = ai->attack_target->z - ai->mob->entity->z;

            if(x_diff * x_diff + y_diff * y_diff + z_diff * z_diff > distance * distance * 4.0 && random_next_int_range(ai->random, 100) == 0) {
                ai->attack_target = NULL;
            }

            if(ai->attack_target != NULL) {
                float dist = (x_diff * x_diff + y_diff * y_diff + z_diff * z_diff);
                ai->mob->entity->y_rot = (atan2f(z_diff, x_diff) * 180.0 / M_PI) - 90.0;
                ai->mob->entity->x_rot = -(atan2f(y_diff, sqrtf(dist)) * 180.0 / M_PI);
                if(sqrtf(dist) < 2.0 && ai->attack_delay == 0) {
                    ai_basic_attack_attack(ai);
                }
            }
        }
    }
}

void ai_basic_attack_hurt(struct ai_s *ai, struct entity_s *entity, int damage) {
    ai_basic_hurt(ai, entity, damage);
    if(entity && entity->type == ENTITY_ARROW) {
        entity = entity->owner;
    }
    if(entity != NULL && entity->type != ai->mob->entity->type) {
        ai->attack_target = entity;
    }
}