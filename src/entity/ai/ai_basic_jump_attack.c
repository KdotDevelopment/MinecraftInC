#include <entity/ai/ai_basic_jump_attack.h>
#include <entity/mob/mob.h>
#include <world/world.h>

#include <stddef.h>

ai_t ai_basic_jump_attack_create(struct world_s *world, struct mob_s *mob) {
    ai_t ai = ai_basic_attack_create(world, mob);
    ai.jump = ai_basic_jump_attack_jump;
    ai.run_speed *= 0.8;

    return ai;
}

void ai_basic_jump_attack_jump(struct ai_s *ai) {
    if(ai->attack_target == NULL) {
        ai_jump(ai);
        return;
    }
    ai->mob->entity->xd = 0;
    ai->mob->entity->zd = 0;
    entity_move_relative(ai->mob->entity, 0, 1, 0);
    ai->mob->entity->yd = 0.5;
}