#include <particle/particle_water_drop.h>
#include <level/level.h>

#include <util/random.h>

particle_t particle_water_drop_create(struct level_s *level, float x, float y, float z) {
    particle_t particle = particle_create(level, x, y, z, 0, 0, 0);
    particle.type = PARTICLE_WATER_DROP;
    particle.entity.xd *= 0.3;
    particle.entity.zd *= 0.3;
    particle.entity.yd = random_uniform() * 0.2 + 0.1;
    particle.r = 1.0;
    particle.g = 1.0;
    particle.b = 1.0;
    particle.texture = 16;
    particle.entity.bb_width = 0.01;
    particle.entity.bb_height = 0.01;
    particle.lifetime = 8.0 / (random_uniform() * 0.8 + 0.2);

    particle.tick = particle_water_drop_tick;

    return particle;
}

void particle_water_drop_tick(particle_t *particle) {
    particle->entity.xo = particle->entity.x;
    particle->entity.yo = particle->entity.y;
    particle->entity.zo = particle->entity.z;
    if(particle->age++ >= particle->lifetime) {
        entity_remove(&particle->entity);
    }
    particle->entity.yd -= 0.06;
    entity_move(&particle->entity, particle->entity.xd, particle->entity.yd, particle->entity.zd);
    particle->entity.xd *= 0.98;
    particle->entity.yd *= 0.98;
    particle->entity.zd *= 0.98;
    if(particle->entity.on_ground) {
        if(random_uniform() < 0.5) entity_remove(&particle->entity);
        particle->entity.xd *= 0.7;
        particle->entity.zd *= 0.7;
    }
}