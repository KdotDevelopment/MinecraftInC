#pragma once

#include <entity/mob/mob.h>

void mob_creeper_create(struct entity_s *entity, struct world_s *world, float x, float y, float z);
float mob_creeper_get_brightness(struct entity_s *entity, float tick);