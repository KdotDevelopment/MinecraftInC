#include <particle/particle_bubble.h>
#include <world/world.h>

#include <util/random.h>

particle_t particle_bubble_create(struct world_s *world, float x, float y, float z, float xd, float yd, float zd) {
    particle_t particle = particle_create(world, x, y, z, xd, yd, zd);
    particle.type = PARTICLE_BUBBLE;
    particle.entity.xd *= 0.2 + (random_uniform() * 2.0 - 1.0) * 0.02;
    particle.entity.yd *= 0.2 + (random_uniform() * 2.0 - 1.0) * 0.02;
    particle.entity.zd *= 0.2 + (random_uniform() * 2.0 - 1.0) * 0.02;
    particle.size *= random_uniform() * 0.6 + 0.2;
    particle.r = 1.0;
    particle.g = 1.0;
    particle.b = 1.0;
    particle.texture = 16;
    particle.entity.bb_width = 0.01;
    particle.entity.bb_height = 0.01;
    particle.lifetime = 8.0 / (random_uniform() * 0.8 + 0.2);

    particle.tick = particle_bubble_tick;

    return particle;
}

void particle_bubble_tick(particle_t *particle) {
    particle->entity.xo = particle->entity.x;
    particle->entity.yo = particle->entity.y;
    particle->entity.zo = particle->entity.z;
    particle->entity.yd += 0.002;
    entity_move(&particle->entity, particle->entity.xd, particle->entity.yd, particle->entity.zd);
    particle->entity.xd *= 0.85;
    particle->entity.yd *= 0.85;
    particle->entity.zd *= 0.85;
    if(world_get_block_material(particle->entity.world, floor_double(particle->entity.x), floor_double(particle->entity.y), floor_double(particle->entity.z)) != &materials.water) {
        entity_remove(&particle->entity);
    }
    if(particle->age++ >= particle->lifetime) {
        entity_remove(&particle->entity);
    }
}