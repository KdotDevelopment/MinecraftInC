#include <renderer/entity/renderer_entity_arrow.h>

#include <minecraft.h>
#include <renderer/entity/renderer_entity_manager.h>
#include <renderer/tesselator.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

renderer_entity_t renderer_entity_arrow_create() {
    renderer_entity_t renderer = renderer_entity_create();

    renderer.render = renderer_entity_arrow_render;

    return renderer;
}

void renderer_entity_arrow_render(renderer_entity_t *renderer, entity_t *entity, double x, double y, double z, float x_rot, float partial_tick) {
    glBindTexture(GL_TEXTURE_2D, textures_load(&renderer->manager->world->minecraft->textures, "item/arrows.png"));
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(entity->y_roto + (entity->y_rot - entity->y_roto) * partial_tick - 90.0, 0, 1, 0);
    glRotatef(entity->x_roto + (entity->x_rot - entity->x_roto) * partial_tick, 0, 0, 1);
    glRotatef(45, 1, 0, 0);
    partial_tick = 0.5;
    float fins_v1 = (0 + entity->arrow_type * 10) / 32.0;
    float stem_v1 = (5 + entity->arrow_type * 10) / 32.0;
    float fins_v2 = (5 + entity->arrow_type * 10) / 32.0;
    float stem_v2 = (10 + entity->arrow_type * 10) / 32.0;
    float u = 0.15625;
    float b = 0.05625;
    glScalef(b, b, b);
    glNormal3f(b, 0, 0);
    tesselator_begin_quads();
    tesselator_vertex_uv(-7, -2, -2, 0, stem_v1);
    tesselator_vertex_uv(-7, -2, 2, u, stem_v1);
    tesselator_vertex_uv(-7, 2, 2, u, stem_v2);
    tesselator_vertex_uv(-7, 2, -2, 0, stem_v2);
    tesselator_end();
    glNormal3f(-b, 0, 0);
    tesselator_begin_quads();
    tesselator_vertex_uv(-7, 2, -2, 0, stem_v1);
    tesselator_vertex_uv(-7, 2, 2, u, stem_v1);
    tesselator_vertex_uv(-7, -2, 2, u, stem_v2);
    tesselator_vertex_uv(-7, -2, -2, 0, stem_v2);
    tesselator_end();
    
    for(int i = 0; i < 4; i++) {
        glRotatef(90, 1, 0, 0);
        glNormal3f(0, -b, 0);
        tesselator_begin_quads(); //not in original? remove if not working
        tesselator_vertex_uv(-8, -2, 0, 0, fins_v1);
        tesselator_vertex_uv(8, -2, 0, partial_tick, fins_v1);
        tesselator_vertex_uv(8, 2, 0, partial_tick, fins_v2);
        tesselator_vertex_uv(-8, 2, 0, 0, fins_v2);
        tesselator_end();
    }

    glColor4f(1, 1, 1, 1);
    glPopMatrix();
}