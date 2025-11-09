#pragma once

#include <renderer/entity/renderer_entity.h>

renderer_entity_t renderer_entity_arrow_create();
void renderer_entity_arrow_render(renderer_entity_t *renderer, entity_t *entity, double x, double y, double z, float x_rot, float partial_tick);