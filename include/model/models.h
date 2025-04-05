#pragma once

#include <model/model.h>

typedef struct models_t {
    model_t creeper;
    model_t pig;
    model_t sheep;
    model_t sheep_fur;
    model_t skeleton;
    model_t spider;
    model_t zombie;
    model_t quadruped;
    model_t humanoid;
    model_t humanoid_armor;
} models_t;

models_t models_create();
model_t *models_get(models_t *models, model_type_t type);