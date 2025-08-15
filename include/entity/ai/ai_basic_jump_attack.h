#pragma once

#include <entity/ai/ai_basic_attack.h>

ai_t ai_basic_jump_attack_create(struct world_s *world, struct mob_s *mob);
void ai_basic_jump_attack_jump(struct ai_s *ai);