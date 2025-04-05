#include <particle/particles.h>
#include <level/level.h>

#include <util/array_list.h>

#include <stdlib.h>

particles_t particles_create(struct level_s *level, textures_t *textures) {
    particles_t particles = { 0 };
    for(int i = 0; i < 2; i++) {
        particles.particles[i] = array_list_create(sizeof(particle_t *));
    }

    return particles;
}

void particles_spawn_particle(particles_t *particles, particle_t *particle) {
    int tex = particle->get_texture(particle);
    particles->particles[tex] = array_list_push(particles->particles[tex], &particle);
}

void particles_tick(particles_t *particles) {
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < array_list_length(particles->particles[i]); j++) {
            particle_t *particle = particles->particles[i][j];
            particle->tick(particle);
            if(particle->entity.removed) {
                particles->particles[i] = array_list_remove(particles->particles[i], j--);
                free(particle);
            }
        }
    }
}

void particles_destroy(particles_t *particles) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < array_list_length(particles->particles[i]); j++) {
            particle_t *particle = particles->particles[i][j];
            free(particle);
        }
        array_list_free(particles->particles[i]);
    }
}