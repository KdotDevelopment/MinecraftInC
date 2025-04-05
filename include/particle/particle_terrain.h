#pragma once

#include <particle/particle.h>
#include <level/block/block.h>

particle_t particle_terrain_create(struct level_s *level, float x, float y, float z, float xd, float yd, float zd, block_t *block);
int particle_terrain_get_texture(particle_t *particle);
void particle_terrain_render(particle_t *particle, float delta, float x, float y, float z, float var6, float var7);