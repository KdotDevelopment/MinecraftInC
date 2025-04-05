#include <entity/ai/ai_skeleton.h>
#include <entity/ai/ai_basic.h>
#include <entity/mob/mob.h>

ai_t ai_skeleton_create(struct level_s *level, struct mob_s *mob) {
    ai_t ai = ai_basic_attack_create(level, mob);
    
    ai.tick = ai_skeleton_tick;

    return ai;
}

void ai_skeleton_tick(ai_t *ai) {
    ai_basic_tick(ai);
    mob_t *mob = (mob_t *)ai->mob;
    if(mob->health > 0 && random_int_range(0, 30) == 0 && ai->attack_target != NULL) {

    }
}