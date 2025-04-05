#pragma once

#include <particle/particle.h>

particle_t particle_water_drop_create(struct level_s *level, float x, float y, float z);
void particle_water_drop_tick(particle_t *particle);