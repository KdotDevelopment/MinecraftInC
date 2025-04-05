#pragma once

#include <entity/mob/mob.h>

void mob_skeleton_create(mob_t *mob, struct level_s *level, float x, float y, float z);
void mob_skeleton_shoot_arrow(mob_t *mob, struct level_s *level);