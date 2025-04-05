#include <model/models.h>

#include <model/model_creeper.h>
#include <model/model_pig.h>
#include <model/model_sheep.h>
#include <model/model_sheep_fur.h>
#include <model/model_skeleton.h>
#include <model/model_spider.h>
#include <model/model_zombie.h>
#include <model/model_quadruped.h>
#include <model/model_humanoid.h>

models_t models_create() {
    models_t models = { 0 };
    models.creeper = model_creeper_create();
    models.pig = model_pig_create();
    models.sheep_fur = model_sheep_fur_create();
    models.sheep = model_sheep_create();
    models.skeleton = model_skeleton_create(0);
    models.spider = model_spider_create();
    models.zombie = model_zombie_create(0);
    models.quadruped = model_quadruped_create(0, 0);
    models.humanoid = model_humanoid_create(0);
    models.humanoid_armor = model_humanoid_create(1);

    return models;
}

model_t *models_get(models_t *models, model_type_t type) {
    switch(type) {
        case MODEL_CREEPER: return &models->creeper;
        case MODEL_PIG: return &models->pig;
        case MODEL_SHEEP: return &models->sheep;
        case MODEL_SHEEP_FUR: return &models->sheep_fur;
        case MODEL_SKELETON: return &models->skeleton;
        case MODEL_SPIDER: return &models->spider;
        case MODEL_ZOMBIE: return &models->zombie;
        case MODEL_QUADRUPED: return &models->quadruped;
        case MODEL_HUMANOID: return &models->humanoid;
        default: return &models->humanoid;
    }
}