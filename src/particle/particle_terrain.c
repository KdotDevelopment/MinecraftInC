#include <particle/particle_terrain.h>
#include <level/level.h>
#include <renderer/tesselator.h>

particle_t particle_terrain_create(struct level_s *level, float x, float y, float z, float xd, float yd, float zd, block_t *block) {
    particle_t particle = particle_create(level, x, y, z, xd, yd, zd);
    particle.type = PARTICLE_TERRAIN;
    particle.texture = block->texture_id;
    particle.gravity = block->particle_gravity;
    particle.r = 0.6;
    particle.g = 0.6;
    particle.b = 0.6;

    particle.get_texture = particle_terrain_get_texture;
    particle.render = particle_terrain_render;

    return particle;
}

int particle_terrain_get_texture(particle_t *particle) {
    return 1;
}

void particle_terrain_render(particle_t *particle, float delta, float x, float y, float z, float var6, float var7) {
    float u0 = ((particle->texture % 16) + particle->u / 4.0) / 16.0;
    float v0 = ((particle->texture / 16) + particle->v / 4.0) / 16.0;
    float u1 = u0 + 0.015609375;
    float v1 = v0 + 0.015609375;
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