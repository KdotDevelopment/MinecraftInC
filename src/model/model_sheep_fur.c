#include <model/model_sheep_fur.h>
#include <model/model_quadruped.h>
#include <model/cube.h>
#include <model/model.h>

#include <math.h>

model_t model_sheep_fur_create() {
    model_t model = model_quadruped_create(12, 0.0);
    model.type = MODEL_SHEEP_FUR;
    model.render = model_sheep_fur_render;

    model.head = cube_create(0, 0);
    cube_add_box(&model.head, -3.0F, -4.0F, -4.0F, 6, 6, 6, 0.6F);
    cube_set_pos(&model.head, 0.0F, 6.0F, -8.0F);
    model.body = cube_create(28, 8);
    cube_add_box(&model.body, -4.0F, -10.0F, -7.0F, 8, 16, 6, 1.75F);
    cube_set_pos(&model.body, 0.0F, 5.0F, 2.0F);
    float var1 = 0.5F;
    model.leg0 = cube_create(0, 16);
    cube_add_box(&model.leg0, -2.0F, 0.0F, -2.0F, 4, 6, 4, var1);
    cube_set_pos(&model.leg0, -3.0F, 12.0F, 7.0F);
    model.leg1 = cube_create(0, 16);
    cube_add_box(&model.leg1, -2.0F, 0.0F, -2.0F, 4, 6, 4, var1);
    cube_set_pos(&model.leg1, 3.0F, 12.0F, 7.0F);
    model.leg2 = cube_create(0, 16);
    cube_add_box(&model.leg2, -2.0F, 0.0F, -2.0F, 4, 6, 4, var1);
    cube_set_pos(&model.leg2, -3.0F, 12.0F, -5.0F);
    model.leg3 = cube_create(0, 16);
    cube_add_box(&model.leg3, -2.0F, 0.0F, -2.0F, 4, 6, 4, var1);
    cube_set_pos(&model.leg3, 3.0F, 12.0F, -5.0F);

    return model;
}

void model_sheep_fur_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model_quadruped_render(model, time, r, bob, y_rot, x_rot, scale);
}