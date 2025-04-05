#pragma once

#include <model/cube.h>
#include <model/model.h>

model_t model_creeper_create();
void model_creeper_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale);
void model_creeper_setup_anim(model_t *model_creeper, float time, float r, float bob, float y_rot, float x_rot, float scale);