#pragma once

#include <model/cube.h>

#include <util/sin_table.h>

typedef enum {
    MODEL_CREEPER,
    MODEL_PIG,
    MODEL_SHEEP,
    MODEL_SHEEP_FUR,
    MODEL_SKELETON,
    MODEL_SPIDER,
    MODEL_ZOMBIE,
    MODEL_HUMANOID,
    MODEL_HUMANOID_ARMOR,
    MODEL_QUADRUPED,
    MODEL_ITEM,
} model_type_t;

typedef struct model_s {
    float attack_time;
    model_type_t type;

    cube_t head; //also for item cube
    cube_t hair;
    cube_t body;

    cube_t body0;
    cube_t body1;

    cube_t arm0;
    cube_t arm1;

    cube_t leg0;
    cube_t leg1;
    cube_t leg2;
    cube_t leg3;
    cube_t leg4;
    cube_t leg5;
    cube_t leg6;
    cube_t leg7;

    void (*render)(struct model_s *model, float time, float r, float bob, float y_rot, float x_rot, float scale);
    void (*setup_anim)(struct model_s *model, float time, float r, float bob, float y_rot, float x_rot, float scale);
} model_t;

model_t model_create();
model_t *model_get(model_type_t model_type);
void model_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale);
void model_setup_anim(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale);