#pragma once

#include <model/cube.h>
#include <model/model.h>
#include <string.h>

model_t model_quadruped_create(int leg_size, float g);
void model_quadruped_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale);
void model_quadruped_setup_anim(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale);