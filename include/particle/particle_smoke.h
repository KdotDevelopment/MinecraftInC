#pragma once

#include <particle/particle.h>

particle_t particle_smoke_create(struct level_s *level, float x, float y, float z);
void particle_smoke_tick(particle_t *particle);