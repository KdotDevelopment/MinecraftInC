#pragma once

#include <renderer/entity/renderer_entity.h>
#include <world/world.h>

struct minecraft_s;

typedef struct renderer_entity_list_s {
    renderer_entity_t spider;
    renderer_entity_t pig;
    renderer_entity_t sheep;
    renderer_entity_t creeper;
    renderer_entity_t skeleton;
    renderer_entity_t zombie;
    renderer_entity_t player;
    renderer_entity_t giant;
    renderer_entity_t mob;
    renderer_entity_t entity;
    renderer_entity_t painting;
    renderer_entity_t arrow;
    renderer_entity_t item;
    renderer_entity_t tnt;
} renderer_entity_list_t;

typedef struct renderer_entity_manager_s {
    renderer_entity_list_t list;
    world_t *world;
    
    double render_x;
    double render_y;
    double render_z;
    float player_rot;
    double tick_pos_x;
    double tick_pos_y;
    double tick_pos_z;
} renderer_entity_manager_t;

void renderer_entity_manager_create(renderer_entity_manager_t *renderer_manager, struct minecraft_s *minecraft);
renderer_entity_t *renderer_entity_manager_get_renderer(renderer_entity_manager_t *manager, entity_t *entity);
void renderer_entity_manager_cache(renderer_entity_manager_t *manager, world_t *world, entity_t *player, float partial_tick);
void renderer_entity_manager_render(renderer_entity_manager_t *manager, entity_t *entity, float partial_tick);
void renderer_entity_manager_render_pos(renderer_entity_manager_t *manager, entity_t *entity, double x, double y, double z, float x_rot, float partial_tick);
double renderer_entity_manager_distance(renderer_entity_manager_t *manager, double x, double y, double z);