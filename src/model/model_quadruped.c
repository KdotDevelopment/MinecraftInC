#include <model/model_quadruped.h>
#include <model/cube.h>
#include <model/model.h>

#include <math.h>

model_t model_quadruped_create(int leg_size, float g) {
    model_t model = model_create();
    model.type = MODEL_QUADRUPED;
    model.render = model_quadruped_render;
    model.setup_anim = model_quadruped_setup_anim;
    
    model.head = cube_create(0, 0);
    cube_add_box(&model.head, -4.0, -4.0, -8.0, 8, 8, 8, 0.0);
    cube_set_pos(&model.head, 0.0, (float)(18 - leg_size), -6.0);
    model.body = cube_create(28, 8);
    cube_add_box(&model.body, -5.0, -10.0, -7.0, 10, 16, 8, 0.0);
    cube_set_pos(&model.body, 0.0F, (float)(17 - leg_size), 2.0);
    model.leg0 = cube_create(0, 16);
    cube_add_box(&model.leg0, -2.0, 0.0, -2.0, 4, leg_size, 4, 0.0);
    cube_set_pos(&model.leg0, -3.0, (float)(24 - leg_size), 7.0);
    model.leg1 = cube_create(0, 16);
    cube_add_box(&model.leg1, -2.0, 0.0, -2.0, 4, leg_size, 4, 0.0);
    cube_set_pos(&model.leg1, 3.0, (float)(24 - leg_size), 7.0);
    model.leg2 = cube_create(0, 16);
    cube_add_box(&model.leg2, -2.0, 0.0, -2.0, 4, leg_size, 4, 0.0);
    cube_set_pos(&model.leg2, -3.0, (float)(24 - leg_size), -5.0);
    model.leg3 = cube_create(0, 16);
    cube_add_box(&model.leg3, -2.0, 0.0, -2.0, 4, leg_size, 4, 0.0);
    cube_set_pos(&model.leg3, 3.0, (float)(24 - leg_size), -5.0);
    return model;
}

void model_quadruped_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model->setup_anim(model, time, r, bob, y_rot, x_rot, scale);
    cube_render(&model->head, scale);
    cube_render(&model->body, scale);
    cube_render(&model->leg0, scale);
    cube_render(&model->leg1, scale);
    cube_render(&model->leg2, scale);
    cube_render(&model->leg3, scale);
}

void model_quadruped_setup_anim(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model->head.y_rot = y_rot / (180.0 / PI);
    model->head.x_rot = x_rot / (180.0 / PI);
    model->body.x_rot = PI / 2;
    model->leg0.x_rot = tcos(time * 0.6662) * 1.4 * r;
    model->leg1.x_rot = tcos(time * 0.6662 + PI) * 1.4 * r;
    model->leg2.x_rot = tcos(time * 0.6662 + PI) * 1.4 * r;
    model->leg3.x_rot = tcos(time * 0.6662) * 1.4 * r;
}