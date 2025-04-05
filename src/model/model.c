#include <model/model.h>

#include <stdlib.h>

model_t model_create() {
    model_t model = { 0 };
    model.attack_time = 0;
    model.render = model_render;
    model.setup_anim = model_setup_anim;
    return model;
}

void model_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
}

void model_setup_anim(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
}