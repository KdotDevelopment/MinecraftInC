#pragma once

#include <particle/particle.h>

particle_t particle_water_drop_create(struct world_s *world, float x, float y, float z);
void particle_water_drop_tick(particle_t *particle);