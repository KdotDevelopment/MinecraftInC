#include <model/model_pig.h>
#include <model/model_quadruped.h>
#include <model/cube.h>
#include <model/model.h>

#include <math.h>

model_t model_pig_create() {
    model_t model = model_quadruped_create(6, 0.0);
    model.render = model_pig_render;
    model.type = MODEL_PIG;

    return model;
}

void model_pig_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model_quadruped_render(model, time, r, bob, y_rot, x_rot, scale);
}