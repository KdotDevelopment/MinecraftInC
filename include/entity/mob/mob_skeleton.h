#pragma once

#include <entity/mob/mob.h>

void mob_skeleton_create(struct entity_s *entity, struct world_s *world, float x, float y, float z);
void mob_skeleton_shoot_arrow(mob_t *mob, struct world_s *world);