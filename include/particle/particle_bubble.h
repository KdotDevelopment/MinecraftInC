#pragma once

#include <particle/particle.h>

particle_t particle_bubble_create(struct world_s *world, float x, float y, float z, float xd, float yd, float zd);
void particle_bubble_tick(particle_t *particle);