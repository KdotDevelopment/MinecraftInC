#pragma once

#include <entity/entity.h>

struct renderer_entity_manager_s;

typedef struct renderer_entity_s {
    struct renderer_entity_manager_s *manager;
    float shadow_size;
    float shadow_opaque;

    void (*render)(struct renderer_entity_s *renderer_entity, entity_t *entity, double x, double y, double z, float rot_x, float partial_tick);
    void (*render_shadow)(struct renderer_entity_s *renderer_entity, entity_t *entity, double x, double y, double z, float partial_tick);
} renderer_entity_t;

renderer_entity_t renderer_entity_create();
void renderer_entity_render(renderer_entity_t *renderer, entity_t *entity, double x, double y, double z, float rot_x, float partial_tick);
void renderer_entity_render_shadow(renderer_entity_t *renderer, entity_t *entity, double x, double y, double z, float partial_tick);
