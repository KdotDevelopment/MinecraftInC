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
    float far_plane_distance;
    float fog_fade;
    float fog_fade_prev;
} renderer_camera_t;

renderer_camera_t renderer_camera_create(minecraft_t *minecraft);
vec3_t renderer_camera_get_player_vector(renderer_camera_t *renderer, float delta);
void renderer_camera_hurt_effect(renderer_camera_t *renderer, float delta);
void renderer_camera_apply_bobbing(renderer_camera_t *renderer, float delta);
void renderer_camera_update(renderer_camera_t *renderer, float delta);
void renderer_camera_update_mouse(renderer_camera_t *renderer, float delta);
void renderer_camera_set_lighting(renderer_camera_t *renderer_camera, uint8_t lighting);
void renderer_camera_setup_gui(renderer_camera_t *renderer_camera);
void renderer_camera_setup_fog(renderer_camera_t *renderer_camera);