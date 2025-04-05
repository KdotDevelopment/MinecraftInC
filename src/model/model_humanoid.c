#include <model/model_humanoid.h>
#include <model/cube.h>
#include <model/model.h>

#include <math.h>

model_t model_humanoid_create(float g) {
    model_t model = model_create();
    model.type = MODEL_HUMANOID;
    model.render = model_humanoid_render;
    model.setup_anim = model_humanoid_setup_anim;
    
    model.head = cube_create(0, 0);
    cube_add_box(&model.head, -4.0F, -8.0F, -4.0F, 8, 8, 8, g);
    model.hair = cube_create(32, 0);
    cube_add_box(&model.hair, -4.0F, -8.0F, -4.0F, 8, 8, 8, g + 0.5F);
    model.body = cube_create(16, 16);
    cube_add_box(&model.body, -4.0F, 0.0F, -2.0F, 8, 12, 4, g);
    model.arm0 = cube_create(40, 16);
    cube_add_box(&model.arm0, -3.0F, -2.0F, -2.0F, 4, 12, 4, g);
    cube_set_pos(&model.arm0, -5.0F, 2.0F, 0.0F);
    model.arm1 = cube_create(40, 16);
    model.arm1.mirror = 1;
    cube_add_box(&model.arm1, -1.0F, -2.0F, -2.0F, 4, 12, 4, g);
    cube_set_pos(&model.arm1, 5.0F, 2.0F, 0.0F);
    model.leg0 = cube_create(0, 16);
    cube_add_box(&model.leg0, -2.0F, 0.0F, -2.0F, 4, 12, 4, g);
    cube_set_pos(&model.leg0, -2.0F, 12.0F, 0.0F);
    model.leg1 = cube_create(0, 16);
    model.leg1.mirror = 1;
    cube_add_box(&model.leg1, -2.0F, 0.0F, -2.0F, 4, 12, 4, g);
    cube_set_pos(&model.leg1, 2.0F, 12.0F, 0.0F);

    return model;
}

void model_humanoid_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model->setup_anim(model, time, r, bob, y_rot, x_rot, scale);
    cube_render(&model->head, scale);
    cube_render(&model->body, scale);
    cube_render(&model->hair, scale);
    cube_render(&model->arm0, scale);
    cube_render(&model->arm1, scale);
    cube_render(&model->leg0, scale);
    cube_render(&model->leg1, scale);
}

void model_humanoid_setup_anim(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model->head.y_rot = y_rot / (180.0 / PI);
    model->head.x_rot = x_rot / (180.0 / PI);
    model->arm0.x_rot = tcos(time * 0.6662 + PI) * 2.0 * r;
    model->arm0.z_rot = (tcos(time * 0.2312) + 1.0) * r;
    model->arm1.x_rot = tcos(time * 0.6662) * 2.0 * r;
    model->arm1.z_rot = (tcos(time * 0.2812) - 1.0) * r;
    model->leg0.x_rot = tcos(time * 0.6662) * 1.4 * r;
    model->leg1.x_rot = tcos(time * 0.6662 + PI) * 1.4 * r;
    model->arm0.z_rot = model->arm0.z_rot + tcos(bob * 0.09) * 0.05 + 0.05;
    model->arm1.z_rot = model->arm1.z_rot - (tcos(bob * 0.09) * 0.05 + 0.05);
    model->arm0.x_rot = model->arm0.x_rot + tsin(bob * 0.067) * 0.05;
    model->arm1.x_rot = model->arm1.x_rot - tsin(bob * 0.067) * 0.05;
}