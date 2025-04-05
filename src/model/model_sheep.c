#include <model/model_sheep.h>
#include <model/model_quadruped.h>
#include <model/cube.h>
#include <model/model.h>

#include <math.h>

model_t model_sheep_create() {
    model_t model = model_quadruped_create(12, 0.0);
    model.type = MODEL_SHEEP_FUR;
    model.render = model_sheep_render;

    model.head = cube_create(0, 0);
    cube_add_box(&model.head, -3.0F, -4.0F, -6.0F, 6, 6, 8, 0.0F);
    cube_set_pos(&model.head, 0.0F, 6.0F, -8.0F);
    model.body = cube_create(28, 8);
    cube_add_box(&model.body, -4.0F, -10.0F, -7.0F, 8, 16, 6, 0.0F);
    cube_set_pos(&model.body, 0.0F, 5.0F, 2.0F);

    return model;
}

void model_sheep_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model_quadruped_render(model, time, r, bob, y_rot, x_rot, scale);
}