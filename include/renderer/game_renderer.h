#pragma once

#include <model/vec3.h>
#include <entity/entity.h>
#include <renderer/held_block.h>

#include <util/random.h>

#include <stdint.h>

typedef struct minecraft_s minecraft_t;

typedef struct {
    minecraft_t *minecraft;
    entity_t *entity;
    held_block_t held_block;
    uint8_t display_active;
    random_t random;
    float fog_r;
    float fog_g;
    float fog_b;
    float fog_end;
    float fog_color_multiplier;
} game_renderer_t;

game_renderer_t game_renderer_create(minecraft_t *minecraft);
vec3_t renderer_get_player_vector(game_renderer_t *renderer, float delta);
void renderer_apply_bobbing(game_renderer_t *renderer, float delta);
void renderer_set_lighting(game_renderer_t *game_renderer, uint8_t lighting);
void renderer_setup_gui_camera(game_renderer_t *game_renderer);
void renderer_setup_fog(game_renderer_t *game_renderer);