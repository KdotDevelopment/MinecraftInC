#pragma once

#include <entity/mob/mob.h>

void mob_humanoid_create(struct entity_s *entity, struct world_s *world, float x, float y, float z);
void mob_humanoid_read_nbt(entity_t *entity, nbt_base_t *nbt);
void mob_humanoid_write_nbt(entity_t *entity, nbt_base_t *nbt);
void mob_humanoid_render_model(struct mob_s *mob, float time, float r, float bob, float y_rot, float x_rot, float scale);