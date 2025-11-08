#include <renderer/renderer_lighting.h>

#include <model/vec3.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

float renderer_lighting_color_buffer[16];

void renderer_lighting_disable() {
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glDisable(GL_COLOR_MATERIAL);
}

void renderer_lighting_enable() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    vec3_t vec = { 0.3, 1.0, -0.7 };
    glLightfv(GL_LIGHT0, GL_POSITION, renderer_lighting_set_color_buffer_d(vec.x, vec.y, vec.z, 0.0));
    glLightfv(GL_LIGHT0, GL_DIFFUSE, renderer_lighting_set_color_buffer(0.5, 0.5, 0.5, 1.0));
    glLightfv(GL_LIGHT0, GL_AMBIENT, renderer_lighting_set_color_buffer(0.0, 0.0, 0.0, 1.0));
    glLightfv(GL_LIGHT0, GL_SPECULAR, renderer_lighting_set_color_buffer(0.0, 0.0, 0.0, 1.0));
    vec = (vec3_t){ -0.7, 1.0, 0.2 };
    glLightfv(GL_LIGHT1, GL_POSITION, renderer_lighting_set_color_buffer_d(vec.x, vec.y, vec.z, 0.0));
    glLightfv(GL_LIGHT1, GL_DIFFUSE, renderer_lighting_set_color_buffer(0.5, 0.5, 0.5, 1.0));
    glLightfv(GL_LIGHT1, GL_AMBIENT, renderer_lighting_set_color_buffer(0.0, 0.0, 0.0, 1.0));
    glLightfv(GL_LIGHT1, GL_SPECULAR, renderer_lighting_set_color_buffer(0.0, 0.0, 0.0, 1.0));
    glShadeModel(GL_SMOOTH);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, renderer_lighting_set_color_buffer(0.5, 0.5, 0.5, 1.0));
}

float *renderer_lighting_set_color_buffer_d(double r, double g, double b, double a) {
    return renderer_lighting_set_color_buffer(r, g, b, 0.0);
}

float *renderer_lighting_set_color_buffer(float r, float g, float b, float a) {
    memset(renderer_lighting_color_buffer, 0, 16 * sizeof(float));

    renderer_lighting_color_buffer[0] = r;
    renderer_lighting_color_buffer[1] = g;
    renderer_lighting_color_buffer[2] = b;
    renderer_lighting_color_buffer[3] = a;

    return renderer_lighting_color_buffer;
}