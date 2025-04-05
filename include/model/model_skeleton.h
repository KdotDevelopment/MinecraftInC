#pragma once

#include <model/cube.h>
#include <model/model.h>
#include <model/model_humanoid.h>
#include <string.h>

model_t model_skeleton_create(float g);
void model_skeleton_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale);
void model_skeleton_setup_anim(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale);