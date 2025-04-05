#pragma once

#include <entity/mob/mob.h>

void mob_humanoid_create(mob_t *mob, struct level_s *level, float x, float y, float z);
void mob_humanoid_render_model(struct mob_s *mob, float time, float r, float bob, float y_rot, float x_rot, float scale);