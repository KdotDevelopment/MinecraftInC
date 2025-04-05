#pragma once

#include <entity/ai/ai_basic.h>

ai_t ai_basic_attack_create(struct level_s *level, struct mob_s *mob);
void ai_basic_attack_update(struct ai_s *ai);
void ai_basic_attack_hurt(struct ai_s *ai, struct entity_s *entity, int damage);
uint8_t ai_basic_attack_attack(struct ai_s *ai);