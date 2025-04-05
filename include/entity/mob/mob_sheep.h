#pragma once

#include <entity/mob/mob.h>

void mob_sheep_create(mob_t *mob, struct level_s *level, float x, float y, float z);
void mob_sheep_die(mob_t *mob, mob_t *causer);
void mob_sheep_ai_step(struct mob_s *mob);
void mob_sheep_hurt(struct entity_s *mob, struct entity_s *causer, int damage);
void mob_sheep_render_model(struct mob_s *mob, float time, float r, float bob, float y_rot, float x_rot, float scale);