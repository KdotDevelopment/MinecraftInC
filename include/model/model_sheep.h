#pragma once

#include <model/cube.h>
#include <model/model.h>
#include <model/model_quadruped.h>

model_t model_sheep_create();
void model_sheep_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale);