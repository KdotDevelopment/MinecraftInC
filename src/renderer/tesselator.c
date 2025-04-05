#include <renderer/tesselator.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

tesselator_t g_tesselator = { 0 };

void tesselator_create(tesselator_t *tesselator) {
    tesselator->len = 3;
    tesselator->vertices = 0;
    tesselator->has_color = 0;
    tesselator->has_texture = 0;
    tesselator->p = 0;
    tesselator->no_color = 0;
}

void tesselator_end() {
    if(g_tesselator.vertices > 0) {
        //clear buffer
        //buffer.put(this.array, 0, this.p)
        //buffer flip
        if(g_tesselator.has_texture && g_tesselator.has_color) {
            glInterleavedArrays(GL_T2F_C3F_V3F, 0, g_tesselator.buffer);
        }else if(g_tesselator.has_texture) {
            glInterleavedArrays(GL_T2F_V3F, 0, g_tesselator.buffer);
        }else if(g_tesselator.has_color) {
            glInterleavedArrays(GL_C3F_V3F, 0, g_tesselator.buffer);
        }else {
            glInterleavedArrays(GL_V3F, 0, g_tesselator.buffer);
        }

        glEnableClientState(GL_VERTEX_ARRAY);
        if(g_tesselator.has_texture) {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        if(g_tesselator.has_color) {
            glEnableClientState(GL_COLOR_ARRAY);
        }

        glDrawArrays(GL_QUADS, 0, g_tesselator.vertices);

        glDisableClientState(GL_VERTEX_ARRAY);
        if(g_tesselator.has_texture) {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        if(g_tesselator.has_color) {
            glDisableClientState(GL_COLOR_ARRAY);
        }
    }

    tesselator_clear();
}

void tesselator_clear() {
    g_tesselator.vertices = 0;
    //clear buffer
    g_tesselator.p = 0;
}

void tesselator_begin() {
    tesselator_clear();
    g_tesselator.has_color = 0;
    g_tesselator.has_texture = 0;
    g_tesselator.no_color = 0;
}

void tesselator_color(float r, float g, float b) {
    if(g_tesselator.no_color) return;
    if(!g_tesselator.has_color) g_tesselator.len += 3;
    g_tesselator.has_color = 1;
    g_tesselator.r = r;
    g_tesselator.g = g;
    g_tesselator.b = b;
}

void tesselator_vertex_uv(float x, float y, float z, float u, float v) {
    if(!g_tesselator.has_texture) g_tesselator.len += 2;
    g_tesselator.has_texture = 1;
    g_tesselator.u = u;
    g_tesselator.v = v;
    tesselator_vertex(x, y, z);
}

void tesselator_vertex(float x, float y, float z) {
    if(g_tesselator.has_texture) {
        g_tesselator.buffer[g_tesselator.p++] = g_tesselator.u;
        g_tesselator.buffer[g_tesselator.p++] = g_tesselator.v;
    }

    if(g_tesselator.has_color) {
        g_tesselator.buffer[g_tesselator.p++] = g_tesselator.r;
        g_tesselator.buffer[g_tesselator.p++] = g_tesselator.g;
        g_tesselator.buffer[g_tesselator.p++] = g_tesselator.b;
    }

    g_tesselator.buffer[g_tesselator.p++] = x;
    g_tesselator.buffer[g_tesselator.p++] = y;
    g_tesselator.buffer[g_tesselator.p++] = z;
    g_tesselator.vertices++;
    if(g_tesselator.vertices % 4 == 0 && g_tesselator.p >= 524288 - (g_tesselator.len * 4)) {
        tesselator_end();
        g_tesselator.len = 3;
    }
}

void tesselator_color_int(uint32_t color) {
    tesselator_color(((color >> 24) & 0xFF) / 255.0, ((color >> 16) & 0xFF) / 255.0, ((color >> 8) & 0xFF) / 255.0);
}

void tesselator_normal(float x, float y, float z) {
    glNormal3f(x, y, z);
}