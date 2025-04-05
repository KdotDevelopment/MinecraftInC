#pragma once

#include <entity/entity.h>

#include <stdint.h>

struct level_s;

enum {
    PARTICLE_NONE,
    PARTICLE_SMOKE,
    PARTICLE_TERRAIN,
    PARTICLE_WATER_DROP,
};

typedef struct particle_s {
    entity_t entity;
    int texture;
    float u;
    float v;
    int age;
    int lifetime;
    float size;
    float gravity;
    float r;
    float g;
    float b;
    uint8_t type;

    void (*tick)(struct particle_s *particle);
    void (*render)(struct particle_s *particle, float t, float x, float y, float z, float var6, float var7);
    int (*get_texture)(struct particle_s *particle);
} particle_t;

particle_t particle_create(struct level_s *level, float x, float y, float z, float xd, float yd, float zd);
void particle_tick(struct particle_s *particle);
void particle_render(struct particle_s *particle, float delta, float x, float y, float z, float var6, float var7);
int particle_get_texture(struct particle_s *particle);
void particle_set_power(struct particle_s *particle, float power);
void particle_set_scale(struct particle_s *particle, float scale);