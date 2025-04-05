#include <model/model_zombie.h>
#include <model/model_humanoid.h>
#include <model/cube.h>
#include <model/model.h>

#include <math.h>

model_t model_zombie_create(float g) {
    model_t model = model_humanoid_create(g);
    model.type = MODEL_ZOMBIE;
    model.render = model_zombie_render;
    model.setup_anim = model_zombie_setup_anim;

    return model;
}

void model_zombie_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model_humanoid_render(model, time, r, bob, y_rot, x_rot, scale);
}

void model_zombie_setup_anim(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model_humanoid_setup_anim(model, time, r, bob, y_rot, x_rot, scale);
    time = tsin(model->attack_time * PI);
    r = tsin((1.0 - (1.0 - model->attack_time) * (1.0 - model->attack_time)) * PI);
    model->arm0.z_rot = 0.0;
    model->arm1.z_rot = 0.0;
    model->arm0.y_rot = -(0.1 - time * 0.6);
    model->arm1.y_rot = 0.1 - time * 0.6;
    model->arm0.x_rot = (-PI / 2);
    model->arm1.x_rot = (-PI / 2);
    model->arm0.x_rot -= time * 1.2 - r * 0.4;
    model->arm1.x_rot -= time * 1.2 - r * 0.4;
    model->arm0.z_rot = model->arm0.z_rot + tcos(bob * 0.09) * 0.05 + 0.05;
    model->arm1.z_rot = model->arm1.z_rot - (tcos(bob * 0.09) * 0.05 + 0.05);
    model->arm0.x_rot = model->arm0.x_rot + tsin(bob * 0.067) * 0.05;
    model->arm1.x_rot = model->arm1.x_rot - tsin(bob * 0.067) * 0.05;
}