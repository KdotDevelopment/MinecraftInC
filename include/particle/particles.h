#pragma once

#include <particle/particle.h>
#include <renderer/textures.h>

typedef struct particles_s {
    particle_t **particles[2]; //arraylist
    textures_t *textures;
} particles_t;

particles_t particles_create(struct level_s *level, textures_t *textures);
void particles_spawn_particle(particles_t *particles, particle_t *particle);
void particles_tick(particles_t *particles);
void particles_destroy(particles_t *particles);