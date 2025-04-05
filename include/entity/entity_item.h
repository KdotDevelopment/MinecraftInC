#pragma once

#include <entity/entity.h>
#include <model/model.h>

extern model_t item_models[256];

void item_models_init();
void entity_item_create(entity_t *entity, struct level_s *level, float x, float y, float z, int texture_id);
void entity_item_tick(struct entity_s *entity);
void entity_item_render(struct entity_s *entity, textures_t *textures, float delta);
void entity_item_player_touch(struct entity_s *entity, entity_t *player);