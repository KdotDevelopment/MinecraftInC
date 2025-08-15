#pragma once

#include <entity/ai/ai_basic_attack.h>

ai_t ai_skeleton_create(struct world_s *world, struct mob_s *mob);
void ai_skeleton_tick(ai_t *ai);