#include <model/model_spider.h>
#include <model/cube.h>
#include <model/model.h>

#include <math.h>

model_t model_spider_create() {
    model_t model = model_create();
    model.type = MODEL_SPIDER;
    model.render = model_spider_render;
    model.setup_anim = model_spider_setup_anim;
    
    model.head = cube_create(32, 4);
    cube_add_box(&model.head, -4.0F, -4.0F, -8.0F, 8, 8, 8, 0.0F);
    cube_set_pos(&model.head, 0.0F, 0.0F, -3.0F);
    model.body0 = cube_create(0, 0);
    cube_add_box(&model.body0, -3.0F, -3.0F, -3.0F, 6, 6, 6, 0.0F);
    model.body1 = cube_create(0, 12);
    cube_add_box(&model.body1, -5.0F, -4.0F, -6.0F, 10, 8, 12, 0.0F);
    cube_set_pos(&model.body1, 0.0F, 0.0F, 9.0F);
    model.leg0 = cube_create(18, 0);
    cube_add_box(&model.leg0, -15.0F, -1.0F, -1.0F, 16, 2, 2, 0.0F);
    cube_set_pos(&model.leg0, -4.0F, 0.0F, 2.0F);
    model.leg1 = cube_create(18, 0);
    cube_add_box(&model.leg1, -1.0F, -1.0F, -1.0F, 16, 2, 2, 0.0F);
    cube_set_pos(&model.leg1, 4.0F, 0.0F, 2.0F);
    model.leg2 = cube_create(18, 0);
    cube_add_box(&model.leg2, -15.0F, -1.0F, -1.0F, 16, 2, 2, 0.0F);
    cube_set_pos(&model.leg2, -4.0F, 0.0F, 1.0F);
    model.leg3 = cube_create(18, 0);
    cube_add_box(&model.leg3, -1.0F, -1.0F, -1.0F, 16, 2, 2, 0.0F);
    cube_set_pos(&model.leg3, 4.0F, 0.0F, 1.0F);
    model.leg4 = cube_create(18, 0);
    cube_add_box(&model.leg4, -15.0F, -1.0F, -1.0F, 16, 2, 2, 0.0F);
    cube_set_pos(&model.leg4, -4.0F, 0.0F, 0.0F);
    model.leg5 = cube_create(18, 0);
    cube_add_box(&model.leg5, -1.0F, -1.0F, -1.0F, 16, 2, 2, 0.0F);
    cube_set_pos(&model.leg5, 4.0F, 0.0F, 0.0F);
    model.leg6 = cube_create(18, 0);
    cube_add_box(&model.leg6, -15.0F, -1.0F, -1.0F, 16, 2, 2, 0.0F);
    cube_set_pos(&model.leg6, -4.0F, 0.0F, -1.0F);
    model.leg7 = cube_create(18, 0);
    cube_add_box(&model.leg7, -1.0F, -1.0F, -1.0F, 16, 2, 2, 0.0F);
    cube_set_pos(&model.leg7, 4.0F, 0.0F, -1.0F);

    return model;
}

void model_spider_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model_spider_setup_anim(model, time, r, bob, y_rot, x_rot, scale);
    cube_render(&model->head, scale);
    cube_render(&model->body0, scale);
    cube_render(&model->body1, scale);
    cube_render(&model->leg0, scale);
    cube_render(&model->leg1, scale);
    cube_render(&model->leg2, scale);
    cube_render(&model->leg3, scale);
    cube_render(&model->leg4, scale);
    cube_render(&model->leg5, scale);
    cube_render(&model->leg6, scale);
    cube_render(&model->leg7, scale);
}

void model_spider_setup_anim(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model->head.y_rot = y_rot / (180.0 / PI);
    model->head.x_rot = x_rot / (180.0 / PI);
    y_rot = PI / 4;
    model->leg0.z_rot = -y_rot;
    model->leg1.z_rot = y_rot;
    model->leg2.z_rot = -y_rot * 0.74;
    model->leg3.z_rot = y_rot * 0.74;
    model->leg4.z_rot = -y_rot * 0.74;
    model->leg5.z_rot = y_rot * 0.74;
    model->leg6.z_rot = -y_rot;
    model->leg7.z_rot = y_rot;
    y_rot = PI / 8;
    model->leg0.y_rot = y_rot * 2.0;
    model->leg1.y_rot = -y_rot * 2.0;
    model->leg2.y_rot = y_rot;
    model->leg3.y_rot = -y_rot;
    model->leg4.y_rot = -y_rot;
    model->leg5.y_rot = y_rot;
    model->leg6.y_rot = -y_rot * 2.0;
    model->leg7.y_rot = y_rot * 2.0;
    y_rot = -(tcos(time * 0.6662 * 2.0) * 0.4) * r;
    x_rot = -(tcos(time * 0.6662 * 2.0 + PI) * 0.4) * r;
    float var7 = -(tcos(time * 0.6662F * 2.0 + (PI / 2)) * 0.4) * r;
    float var8 = -(tcos(time * 0.6662 * 2.0 + (PI * 3.0 / 2.0)) * 0.4) * r;
    float var9 = fabs(tsin(time * 0.6662) * 0.4F) * r;
    float var10 = fabs(tsin(time * 0.6662 + PI) * 0.4) * r;
    float var11 = fabs(tsin(time * 0.6662 + (PI / 2)) * 0.4) * r;
    r = fabs(tsin(time * 0.6662 + (PI * 3.0 / 2.0)) * 0.4) * r;
    model->leg0.y_rot += y_rot;
    model->leg1.y_rot -= y_rot;
    model->leg2.y_rot += x_rot;
    model->leg3.y_rot -= x_rot;
    model->leg4.y_rot += var7;
    model->leg5.y_rot -= var7;
    model->leg6.y_rot += var8;
    model->leg7.y_rot -= var8;
    model->leg0.z_rot += var9;
    model->leg1.z_rot -= var9;
    model->leg2.z_rot += var10;
    model->leg3.z_rot -= var10;
    model->leg4.z_rot += var11;
    model->leg5.z_rot -= var11;
    model->leg6.z_rot += r;
    model->leg7.z_rot -= r;
}