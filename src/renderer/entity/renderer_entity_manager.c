#include <renderer/entity/renderer_entity_manager.h>

#include <minecraft.h>
#include <model/models.h>
#include <renderer/entity/renderer_entity_arrow.h>
#include <renderer/entity/renderer_entity_creeper.h>
#include <renderer/entity/renderer_entity_entity.h>
#include <renderer/entity/renderer_entity_giant.h>
#include <renderer/entity/renderer_entity_item.h>
#include <renderer/entity/renderer_entity_mob.h>
#include <renderer/entity/renderer_entity_painting.h>
#include <renderer/entity/renderer_entity_player.h>
#include <renderer/entity/renderer_entity_sheep.h>
#include <renderer/entity/renderer_entity_spider.h>
#include <renderer/entity/renderer_entity_tnt.h>

void renderer_entity_manager_create(renderer_entity_manager_t *renderer_manager, minecraft_t *minecraft) {
    memset(renderer_manager, 0, sizeof(renderer_entity_manager_t));

    renderer_manager->list.spider = renderer_entity_spider_create();
    renderer_manager->list.pig = renderer_entity_mob_create(models_get(&minecraft->models, MODEL_PIG), 0.7);
    renderer_manager->list.sheep = renderer_entity_sheep_create(models_get(&minecraft->models, MODEL_SHEEP), models_get(&minecraft->models, MODEL_SHEEP_FUR), 0.7);
    renderer_manager->list.creeper = renderer_entity_creeper_create();
    renderer_manager->list.skeleton = renderer_entity_mob_create(models_get(&minecraft->models, MODEL_SKELETON), 0.5);
    renderer_manager->list.zombie = renderer_entity_mob_create(models_get(&minecraft->models, MODEL_ZOMBIE), 0.5);
    renderer_manager->list.player = renderer_entity_player_create();
    renderer_manager->list.giant = renderer_entity_giant_create(models_get(&minecraft->models, MODEL_ZOMBIE), 0.5, 6.0);
    renderer_manager->list.mob = renderer_entity_mob_create(models_get(&minecraft->models, MODEL_HUMANOID), 0.5);
    renderer_manager->list.entity = renderer_entity_entity_create();
    renderer_manager->list.painting = renderer_entity_painting_create();
    renderer_manager->list.arrow = renderer_entity_arrow_create();
    renderer_manager->list.item = renderer_entity_item_create();
    renderer_manager->list.tnt = renderer_entity_tnt_create();

    renderer_manager->list.spider.manager = renderer_manager;
    renderer_manager->list.pig.manager = renderer_manager;
    renderer_manager->list.sheep.manager = renderer_manager;
    renderer_manager->list.creeper.manager = renderer_manager;
    renderer_manager->list.skeleton.manager = renderer_manager;
    renderer_manager->list.zombie.manager = renderer_manager;
    renderer_manager->list.player.manager = renderer_manager;
    renderer_manager->list.giant.manager = renderer_manager;
    renderer_manager->list.mob.manager = renderer_manager;
    renderer_manager->list.entity.manager = renderer_manager;
    renderer_manager->list.painting.manager = renderer_manager;
    renderer_manager->list.arrow.manager = renderer_manager;
    renderer_manager->list.item.manager = renderer_manager;
    renderer_manager->list.tnt.manager = renderer_manager;
}

renderer_entity_t *renderer_entity_manager_get_renderer(renderer_entity_manager_t *manager, entity_t *entity) {
    switch(entity->type) {
        case ENTITY_ARROW: return &manager->list.arrow;
        case ENTITY_MOB_CREEPER: return &manager->list.creeper;
        case ENTITY_ENTITY: return &manager->list.entity;
        case ENTITY_MOB_GIANT: return &manager->list.giant;
        case ENTITY_ITEM: return &manager->list.item;
        case ENTITY_MOB: return &manager->list.mob;
        case ENTITY_PAINTING: return &manager->list.painting;
        case ENTITY_MOB_PLAYER: return &manager->list.player;
        case ENTITY_MOB_SHEEP: return &manager->list.sheep;
        case ENTITY_MOB_SPIDER: return &manager->list.spider;
        case ENTITY_PRIMED_TNT: return &manager->list.tnt;
        default: return &manager->list.entity;
    }
}

void renderer_entity_manager_cache(renderer_entity_manager_t *manager, world_t *world, entity_t *player, float partial_tick) {
    manager->world = world;
    manager->player_rot = player->y_roto + (player->y_rot - player->y_roto) * partial_tick;
    manager->tick_pos_x = player->last_tick_x + (player->x - player->last_tick_x) * partial_tick;
    manager->tick_pos_y = player->last_tick_y + (player->y - player->last_tick_y) * partial_tick;
    manager->tick_pos_z = player->last_tick_z + (player->z - player->last_tick_z) * partial_tick;
}

void renderer_entity_manager_render(renderer_entity_manager_t *manager, entity_t *entity, float partial_tick) {
    double x = entity->last_tick_x + (entity->x - entity->last_tick_x) * partial_tick;
    double y = entity->last_tick_y + (entity->y - entity->last_tick_y) * partial_tick;
    double z = entity->last_tick_z + (entity->z - entity->last_tick_z) * partial_tick;
    double x_rot = entity->x_roto + (entity->x_rot - entity->x_roto) * partial_tick;
    float brightness = entity_get_brightness(entity, partial_tick);
    glColor3f(brightness, brightness, brightness);
    renderer_entity_manager_render_pos(manager, entity, x - manager->render_x, y - manager->render_y, z - manager->render_z, x_rot, partial_tick);
}

void renderer_entity_manager_render_pos(renderer_entity_manager_t *manager, entity_t *entity, double x, double y, double z, float x_rot, float partial_tick) {
    renderer_entity_t *renderer = renderer_entity_manager_get_renderer(manager, entity);
    if(renderer) {
        renderer->render(renderer, entity, x, y, z, x_rot, partial_tick);
        renderer->render_shadow(renderer, entity, x, y, z, partial_tick);
    }
}

double renderer_entity_manager_distance(renderer_entity_manager_t *manager, double x, double y, double z) {
    double xx = x - manager->tick_pos_x;
    double yy = y - manager->tick_pos_y;
    double zz = z - manager->tick_pos_z;

    return xx * xx + yy * yy + zz * zz;
}