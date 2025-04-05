#pragma once

#include <entity/ai/ai.h>

struct player_s;

ai_t player_ai_create(struct player_s *player);
void player_ai_tick(struct ai_s *proto_ai);
void player_ai_update(struct ai_s *proto_ai);