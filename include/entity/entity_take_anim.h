#pragma once

#include <entity/entity.h>

void entity_take_anim_create(entity_t *entity, struct level_s *level, entity_t *item, struct player_s *player);
void entity_take_anim_tick(entity_t *entity);
void entity_take_anim_render(entity_t *entity, textures_t *textures, float delta);