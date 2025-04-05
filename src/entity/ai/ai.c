#include <entity/ai/ai.h>

ai_t ai_create() {
    ai_t ai = { 0 };
    ai.tick = ai_tick;
    ai.before_removal = ai_before_removal;
    ai.hurt = ai_hurt;
    ai.jump = ai_jump;
    ai.attack = ai_attack;

    return ai;
}

void ai_tick(struct ai_s *ai) {

}

void ai_before_removal(struct ai_s *ai) {

}

void ai_hurt(struct ai_s *ai, struct entity_s *entity, int damage) {

}

void ai_jump(struct ai_s *ai) {

}

uint8_t ai_attack(struct ai_s *ai) {
    return 0;
}