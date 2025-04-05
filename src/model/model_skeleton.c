#include <model/model_skeleton.h>
#include <model/model_zombie.h>
#include <model/cube.h>
#include <model/model.h>

#include <math.h>

model_t model_skeleton_create(float g) {
    model_t model = model_zombie_create(0);
    model.type = MODEL_SKELETON;
    model.render = model_skeleton_render;
    model.setup_anim = model_skeleton_setup_anim;
    
    model.arm0 = cube_create(40, 16);
    cube_add_box(&model.arm0, -1.0F, -2.0F, -1.0F, 2, 12, 2, 0.0F);
    cube_set_pos(&model.arm0, -5.0F, 2.0F, 0.0F);
    model.arm1 = cube_create(40, 16);
    model.arm1.mirror = 1;
    cube_add_box(&model.arm1, -1.0F, -2.0F, -1.0F, 2, 12, 2, 0.0F);
    cube_set_pos(&model.arm1, 5.0F, 2.0F, 0.0F);
    model.leg0 = cube_create(0, 16);
    cube_add_box(&model.leg0, -1.0F, 0.0F, -1.0F, 2, 12, 2, 0.0F);
    cube_set_pos(&model.leg0, -2.0F, 12.0F, 0.0F);
    model.leg1 = cube_create(0, 16);
    model.leg1.mirror = 1;
    cube_add_box(&model.leg1, -1.0F, 0.0F, -1.0F, 2, 12, 2, 0.0F);
    cube_set_pos(&model.leg1, 2.0F, 12.0F, 0.0F);
    
    return model;
}

void model_skeleton_render(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model_zombie_render(model, time, r, bob, y_rot, x_rot, scale);
}

void model_skeleton_setup_anim(model_t *model, float time, float r, float bob, float y_rot, float x_rot, float scale) {
    model_zombie_setup_anim(model, time, r, bob, y_rot, x_rot, scale);
}