#pragma once

#include <entity/entity.h>

void entity_arrow_create(entity_t *entity, struct level_s *level, entity_t *source_entity, float x, float y, float z, float xd, float yd, float zd);
void entity_arrow_tick(struct entity_s *entity);
void entity_arrow_render(struct entity_s *entity, textures_t *textures, float delta);
void entity_arrow_award_kill_score(struct entity_s *entity, struct entity_s *causer, int score);
void entity_arrow_player_touch(struct entity_s *entity, entity_t *player);
entity_t *entity_arrow_get_owner(entity_t *entity);