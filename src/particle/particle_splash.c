#include <particle/particle_splash.h>

#include <particle/particle_water_drop.h>
#include <world/world.h>

#include <util/random.h>

particle_t particle_splash_create(struct world_s *world, float x, float y, float z) {
    particle_t particle = particle_water_drop_create(world, x, y, z);
    particle.type = PARTICLE_SPLASH;
    particle.gravity = 0.04;
    particle.texture++;

    return particle;
}