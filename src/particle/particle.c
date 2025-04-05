#include <particle/particle.h>
#include <entity/entity.h>
#include <renderer/tesselator.h>

#include <util/random.h>

particle_t particle_create(struct level_s *level, float x, float y, float z, float xd, float yd, float zd) {
    particle_t particle = { 0 };
    entity_create(&particle.entity, level);
    particle.entity.bb_width = 0.2;
    particle.entity.bb_height = 0.2;
    entity_set_pos(&particle.entity, x, y, z);
    particle.entity.height_offset = particle.entity.bb_height / 2.0;
    particle.entity.make_step_sound = 0;
    particle.entity.xd = xd + (random_uniform() * 2.0 - 1.0) * 0.4;
    particle.entity.yd = yd + (random_uniform() * 2.0 - 1.0) * 0.4;
    particle.entity.zd = zd + (random_uniform() * 2.0 - 1.0) * 0.4;
    particle.r = 1;
    particle.g = 1;
    particle.b = 1;
    particle.u = random_uniform() * 3.0;
    particle.v = random_uniform() * 3.0;
    particle.size = random_uniform() * 0.5 + 0.5;
    particle.lifetime = 4.0 / (random_uniform() * 0.9 + 0.1);
    particle.age = 0;
    float rnd = (random_uniform() + random_uniform() + 1) * 0.15;
    float magnitude = sqrtf(particle.entity.xd * particle.entity.xd + particle.entity.yd * particle.entity.yd + particle.entity.zd * particle.entity.zd);
    particle.entity.xd = particle.entity.xd / magnitude * rnd * 0.4;
    particle.entity.yd = particle.entity.yd / magnitude * rnd * 0.4 + 0.1;
    particle.entity.zd = particle.entity.zd / magnitude * rnd * 0.4;
    particle.entity.yd += 0.1;

    particle.tick = particle_tick;
    particle.render = particle_render;
    particle.get_texture = particle_get_texture;

    return particle;
}

void particle_tick(struct particle_s *particle) {
    particle->entity.xo = particle->entity.x;
    particle->entity.yo = particle->entity.y;
    particle->entity.zo = particle->entity.z;
    if(particle->age++ >= particle->lifetime) {
        entity_remove(&particle->entity);
    }
    particle->entity.yd -= 0.04 * particle->gravity;
    entity_move(&particle->entity, particle->entity.xd, particle->entity.yd, particle->entity.zd);
    particle->entity.xd *= 0.98;
    particle->entity.yd *= 0.98;
    particle->entity.zd *= 0.98;
    if(particle->entity.on_ground) {
        particle->entity.xd *= 0.7;
        particle->entity.zd *= 0.7;
    }
}

void particle_render(struct particle_s *particle, float delta, float x, float y, float z, float var6, float var7) {
    float u0 = ((particle->texture % 16) + particle->u / 4.0) / 16.0;
    float v0 = ((particle->texture / 16) + particle->v / 4.0) / 16.0;
    float u1 = u0 + 0.0624375;
    float v1 = v0 + 0.0624375;
    float s = 0.1 * particle->size;
    float vx = particle->entity.xo + (particle->entity.x - particle->entity.xo) * delta;
    float vy = particle->entity.yo + (particle->entity.y - particle->entity.yo) * delta;
    float vz = particle->entity.zo + (particle->entity.z - particle->entity.zo) * delta;
    float brightness = entity_get_brightness(&particle->entity, delta);
    tesselator_color(brightness * particle->r, brightness * particle->g, brightness * particle->b);
    tesselator_vertex_uv(vx - x * s - var6 * s, vy - y * s, vz - z * s - var7 * s, u0, v1);
	tesselator_vertex_uv(vx - x * s + var6 * s, vy + y * s, vz - z * s + var7 * s, u0, v0);
	tesselator_vertex_uv(vx + x * s + var6 * s, vy + y * s, vz + z * s + var7 * s, u1, v0);
	tesselator_vertex_uv(vx + x * s - var6 * s, vy - y * s, vz + z * s - var7 * s, u1, v1);
}

int particle_get_texture(struct particle_s *particle) {
    return 0;
}

void particle_set_power(struct particle_s *particle, float power) {
    particle->entity.xd *= power;
    particle->entity.yd = (particle->entity.yd - 0.1) * power + 0.1;
    particle->entity.zd *= power;
}

void particle_set_scale(struct particle_s *particle, float scale) {
    particle->entity.bb_width = scale * 0.2;
    particle->entity.bb_height = scale * 0.2;
    particle->size *= scale;
}