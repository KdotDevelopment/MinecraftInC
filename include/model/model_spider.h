#pragma once

#include <model/cube.h>
#include <model/model.h>
#include <string.h>

model_t model_spider_create();
void model_spider_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale);
void model_spider_setup_anim(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale);