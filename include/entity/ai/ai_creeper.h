#pragma once

#include <entity/ai/ai_basic_attack.h>

ai_t ai_creeper_create(struct mob_s *mob);
uint8_t ai_creeper_attack(ai_t *ai);
void ai_creeper_before_removal(ai_t *ai);