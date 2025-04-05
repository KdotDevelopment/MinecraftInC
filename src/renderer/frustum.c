#include <renderer/frustum.h>
#include <model/vec3.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

void frustum_normalize(frustum_t *frustum, int plane) {
    float length = vec3_length((vec3_t){ frustum->planes[plane][0], frustum->planes[plane][1], frustum->planes[plane][2] });
    for (int i = 0; i < 4; i++) {
        frustum->planes[plane][i] /= length;
    }
}

frustum_t frustum_get() {
    frustum_t frustum = { 0 };
    glGetFloatv(GL_PROJECTION_MATRIX, frustum.projection);
    glGetFloatv(GL_MODELVIEW_MATRIX, frustum.model_view);
    frustum.clip[0x0] = frustum.model_view[0x0] * frustum.projection[0x0] + frustum.model_view[0x1] * frustum.projection[0x4] + frustum.model_view[0x2] * frustum.projection[0x8] + frustum.model_view[0x3] * frustum.projection[0xC];
    frustum.clip[0x1] = frustum.model_view[0x0] * frustum.projection[0x1] + frustum.model_view[0x1] * frustum.projection[0x5] + frustum.model_view[0x2] * frustum.projection[0x9] + frustum.model_view[0x3] * frustum.projection[0xD];
    frustum.clip[0x2] = frustum.model_view[0x0] * frustum.projection[0x2] + frustum.model_view[0x1] * frustum.projection[0x6] + frustum.model_view[0x2] * frustum.projection[0xA] + frustum.model_view[0x3] * frustum.projection[0xE];
    frustum.clip[0x3] = frustum.model_view[0x0] * frustum.projection[0x3] + frustum.model_view[0x1] * frustum.projection[0x7] + frustum.model_view[0x2] * frustum.projection[0xB] + frustum.model_view[0x3] * frustum.projection[0xF];
    frustum.clip[0x4] = frustum.model_view[0x4] * frustum.projection[0x0] + frustum.model_view[0x5] * frustum.projection[0x4] + frustum.model_view[0x6] * frustum.projection[0x8] + frustum.model_view[0x7] * frustum.projection[0xC];
    frustum.clip[0x5] = frustum.model_view[0x4] * frustum.projection[0x1] + frustum.model_view[0x5] * frustum.projection[0x5] + frustum.model_view[0x6] * frustum.projection[0x9] + frustum.model_view[0x7] * frustum.projection[0xD];
    frustum.clip[0x6] = frustum.model_view[0x4] * frustum.projection[0x2] + frustum.model_view[0x5] * frustum.projection[0x6] + frustum.model_view[0x6] * frustum.projection[0xA] + frustum.model_view[0x7] * frustum.projection[0xE];
    frustum.clip[0x7] = frustum.model_view[0x4] * frustum.projection[0x3] + frustum.model_view[0x5] * frustum.projection[0x7] + frustum.model_view[0x6] * frustum.projection[0xB] + frustum.model_view[0x7] * frustum.projection[0xF];
    frustum.clip[0x8] = frustum.model_view[0x8] * frustum.projection[0x0] + frustum.model_view[0x9] * frustum.projection[0x4] + frustum.model_view[0xA] * frustum.projection[0x8] + frustum.model_view[0xB] * frustum.projection[0xC];
    frustum.clip[0x9] = frustum.model_view[0x8] * frustum.projection[0x1] + frustum.model_view[0x9] * frustum.projection[0x5] + frustum.model_view[0xA] * frustum.projection[0x9] + frustum.model_view[0xB] * frustum.projection[0xD];
    frustum.clip[0xA] = frustum.model_view[0x8] * frustum.projection[0x2] + frustum.model_view[0x9] * frustum.projection[0x6] + frustum.model_view[0xA] * frustum.projection[0xA] + frustum.model_view[0xB] * frustum.projection[0xE];
    frustum.clip[0xB] = frustum.model_view[0x8] * frustum.projection[0x3] + frustum.model_view[0x9] * frustum.projection[0x7] + frustum.model_view[0xA] * frustum.projection[0xB] + frustum.model_view[0xB] * frustum.projection[0xF];
    frustum.clip[0xC] = frustum.model_view[0xC] * frustum.projection[0x0] + frustum.model_view[0xD] * frustum.projection[0x4] + frustum.model_view[0xE] * frustum.projection[0x8] + frustum.model_view[0xF] * frustum.projection[0xC];
    frustum.clip[0xD] = frustum.model_view[0xC] * frustum.projection[0x1] + frustum.model_view[0xD] * frustum.projection[0x5] + frustum.model_view[0xE] * frustum.projection[0x9] + frustum.model_view[0xF] * frustum.projection[0xD];
    frustum.clip[0xE] = frustum.model_view[0xC] * frustum.projection[0x2] + frustum.model_view[0xD] * frustum.projection[0x6] + frustum.model_view[0xE] * frustum.projection[0xA] + frustum.model_view[0xF] * frustum.projection[0xE];
    frustum.clip[0xF] = frustum.model_view[0xC] * frustum.projection[0x3] + frustum.model_view[0xD] * frustum.projection[0x7] + frustum.model_view[0xE] * frustum.projection[0xB] + frustum.model_view[0xF] * frustum.projection[0xF];
    frustum.planes[0][0] = frustum.clip[0x3] - frustum.clip[0x0];
    frustum.planes[0][1] = frustum.clip[0x7] - frustum.clip[0x4];
    frustum.planes[0][2] = frustum.clip[0xB] - frustum.clip[0x8];
    frustum.planes[0][3] = frustum.clip[0xF] - frustum.clip[0xC];
    frustum_normalize(&frustum, 0);
    frustum.planes[1][0] = frustum.clip[0x3] + frustum.clip[0x0];
    frustum.planes[1][1] = frustum.clip[0x7] + frustum.clip[0x4];
    frustum.planes[1][2] = frustum.clip[0xB] + frustum.clip[0x8];
    frustum.planes[1][3] = frustum.clip[0xF] + frustum.clip[0xC];
    frustum_normalize(&frustum, 1);
    frustum.planes[2][0] = frustum.clip[0x3] - frustum.clip[0x1];
    frustum.planes[2][1] = frustum.clip[0x7] - frustum.clip[0x5];
    frustum.planes[2][2] = frustum.clip[0xB] - frustum.clip[0x9];
    frustum.planes[2][3] = frustum.clip[0xF] - frustum.clip[0xD];
    frustum_normalize(&frustum, 2);
    frustum.planes[3][0] = frustum.clip[0x3] + frustum.clip[0x1];
    frustum.planes[3][1] = frustum.clip[0x7] + frustum.clip[0x5];
    frustum.planes[3][2] = frustum.clip[0xB] + frustum.clip[0x9];
    frustum.planes[3][3] = frustum.clip[0xF] + frustum.clip[0xD];
    frustum_normalize(&frustum, 3);
    frustum.planes[4][0] = frustum.clip[0x3] - frustum.clip[0x2];
    frustum.planes[4][1] = frustum.clip[0x7] - frustum.clip[0x6];
    frustum.planes[4][2] = frustum.clip[0xB] - frustum.clip[0xA];
    frustum.planes[4][3] = frustum.clip[0xF] - frustum.clip[0xE];
    frustum_normalize(&frustum, 4);
    frustum.planes[5][0] = frustum.clip[0x3] + frustum.clip[0x2];
    frustum.planes[5][1] = frustum.clip[0x7] + frustum.clip[0x6];
    frustum.planes[5][2] = frustum.clip[0xB] + frustum.clip[0xA];
    frustum.planes[5][3] = frustum.clip[0xF] + frustum.clip[0xE];
    frustum_normalize(&frustum, 5);
    return frustum;
}

uint8_t frustum_contains_box(frustum_t frustum, float ax, float ay, float az, float bx, float by, float bz) {
    for (int i = 0; i < 6; i++) {
        uint8_t b = 1;
        b = b && frustum.planes[i][0] * ax + frustum.planes[i][1] * ay + frustum.planes[i][2] * az + frustum.planes[i][3] <= 0.0;
        b = b && frustum.planes[i][0] * bx + frustum.planes[i][1] * ay + frustum.planes[i][2] * az + frustum.planes[i][3] <= 0.0;
        b = b && frustum.planes[i][0] * ax + frustum.planes[i][1] * by + frustum.planes[i][2] * az + frustum.planes[i][3] <= 0.0;
        b = b && frustum.planes[i][0] * bx + frustum.planes[i][1] * by + frustum.planes[i][2] * az + frustum.planes[i][3] <= 0.0;
        b = b && frustum.planes[i][0] * ax + frustum.planes[i][1] * ay + frustum.planes[i][2] * bz + frustum.planes[i][3] <= 0.0;
        b = b && frustum.planes[i][0] * bx + frustum.planes[i][1] * ay + frustum.planes[i][2] * bz + frustum.planes[i][3] <= 0.0;
        b = b && frustum.planes[i][0] * ax + frustum.planes[i][1] * by + frustum.planes[i][2] * bz + frustum.planes[i][3] <= 0.0;
        b = b && frustum.planes[i][0] * bx + frustum.planes[i][1] * by + frustum.planes[i][2] * bz + frustum.planes[i][3] <= 0.0;
        if (b) { return 0; }
    }
    return 1;
}