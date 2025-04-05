#pragma once

#include <entity/ai/ai.h>

ai_t ai_basic_create(struct level_s *level, struct mob_s *mob);
void ai_basic_tick(struct ai_s *ai);
void ai_basic_jump(ai_t *ai);
void ai_basic_update(struct ai_s *ai);
void ai_basic_hurt(struct ai_s *ai, struct entity_s *entity, int damage);