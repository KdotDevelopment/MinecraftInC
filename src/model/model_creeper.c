#include <model/model_creeper.h>
#include <model/cube.h>
#include <model/model.h>

#include <math.h>

model_t model_creeper_create() {
    model_t model = model_create();
    model.type = MODEL_CREEPER;
    model.render = model_creeper_render;
    model.setup_anim = model_creeper_setup_anim;
    
    model.head = cube_create(0, 0);
    cube_add_box(&model.head, -4.0F, -8.0F, -4.0F, 8, 8, 8, 0.0F);
    model.hair = cube_create(32, 0);
    cube_add_box(&model.hair, -4.0F, -8.0F, -4.0F, 8, 8, 8, 0.0F + 0.5F);
    model.body = cube_create(16, 16);
    cube_add_box(&model.body, -4.0F, 0.0F, -2.0F, 8, 12, 4, 0.0F);
    model.leg0 = cube_create(0, 16);
    cube_add_box(&model.leg0, -2.0F, 0.0F, -2.0F, 4, 6, 4, 0.0F);
    cube_set_pos(&model.leg0, -2.0F, 12.0F, 4.0F);
    model.leg1 = cube_create(0, 16);

    cube_add_box(&model.leg1, -2.0F, 0.0F, -2.0F, 4, 6, 4, 0.0F);
    cube_set_pos(&model.leg1, 2.0F, 12.0F, 4.0F);
    model.leg2 = cube_create(0, 16);
    cube_add_box(&model.leg2, -2.0F, 0.0F, -2.0F, 4, 6, 4, 0.0F);
    cube_set_pos(&model.leg2, -2.0F, 12.0F, -4.0F);
    model.leg3 = cube_create(0, 16);
    cube_add_box(&model.leg3, -2.0F, 0.0F, -2.0F, 4, 6, 4, 0.0F);
    cube_set_pos(&model.leg3, 2.0F, 12.0F, -4.0F);
    return model;
}

void model_creeper_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model_creeper_setup_anim(model, time, r, bob, y_rot, x_rot, scale);
    cube_render(&model->head, scale);
    cube_render(&model->body, scale);
    cube_render(&model->leg0, scale);
    cube_render(&model->leg1, scale);
    cube_render(&model->leg2, scale);
    cube_render(&model->leg3, scale);
}

void model_creeper_setup_anim(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model->head.y_rot = y_rot / (180.0 / PI);
    model->head.x_rot = x_rot / (180.0 / PI);
    model->leg0.x_rot = tcos(time * 0.6662) * 1.4 * r;
    model->leg1.x_rot = tcos(time * 0.6662 + PI) * 1.4 * r;
    model->leg2.x_rot = tcos(time * 0.6662 + PI) * 1.4 * r;
    model->leg3.x_rot = tcos(time * 0.6662) * 1.4 * r;
}