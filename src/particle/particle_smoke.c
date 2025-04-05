#include <particle/particle_smoke.h>
#include <level/level.h>

particle_t particle_smoke_create(struct level_s *level, float x, float y, float z) {
    particle_t particle = particle_create(level, x, y, z, 0, 0, 0);
    particle.type = PARTICLE_SMOKE;
    particle.entity.xd *= 0.1;
    particle.entity.yd *= 0.1;
    particle.entity.zd *= 0.1;
    particle.r = random_uniform() * 0.3;
    particle.g = particle.r;
    particle.b = particle.r;
    particle.lifetime = 8.0 / (random_uniform() * 0.8 + 0.2);
    particle.entity.no_physics = 1;

    particle.tick = particle_smoke_tick;

    return particle;
}

void particle_smoke_tick(particle_t *particle) {
    particle->entity.xo = particle->entity.x;
    particle->entity.yo = particle->entity.y;
    particle->entity.zo = particle->entity.z;
    if(particle->age++ >= particle->lifetime) {
        entity_remove(&particle->entity);
    }
    particle->texture = 7 - (particle->age * 8 / particle->lifetime);
    particle->entity.yd += 0.004;
    entity_move(&particle->entity, particle->entity.xd, particle->entity.yd, particle->entity.zd);
    particle->entity.xd *= 0.96;
    particle->entity.yd *= 0.96;
    particle->entity.zd *= 0.96;
    if(particle->entity.on_ground) {
        particle->entity.xd *= 0.7;
        particle->entity.yd *= 0.7;
        particle->entity.zd *= 0.7;
    }
}