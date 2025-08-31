#include <renderer/tesselator.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

tesselator_t g_tesselator = { 0 };

void tesselator_create(tesselator_t *tesselator) {
    tesselator->vertex_count = 0;
    tesselator->raw_buffer_index = 0;
    tesselator->added_vertices = 0;
    tesselator->draw_mode = 0;
    tesselator->x_offset = 0;
    tesselator->y_offset = 0;
    tesselator->z_offset = 0;
    tesselator->vbo_index = 0;
    tesselator->convert_quads_to_triangles = 0;
    tesselator->try_vbo = 0;
    tesselator->is_drawing = 0;
    tesselator->use_vbo = 0;
    tesselator->has_color = 0;
    tesselator->has_texture = 0;
    tesselator->no_color = 0;

    if(tesselator->use_vbo) glGenBuffers(VBO_COUNT, tesselator->vertex_buffer);
}

void tesselator_end() {
    if(!g_tesselator.is_drawing) printf("Not tesselating!\n");
    g_tesselator.is_drawing = 0;
    if(g_tesselator.vertex_count > 0) {
        memcpy(g_tesselator.byte_buffer, g_tesselator.raw_buffer, g_tesselator.raw_buffer_index * sizeof(int));

        if(g_tesselator.use_vbo) {
            g_tesselator.vbo_index = (g_tesselator.vbo_index + 1) % VBO_COUNT;
            glBindBuffer(GL_ARRAY_BUFFER, g_tesselator.vertex_buffer[g_tesselator.vbo_index]);
            glBufferData(GL_ARRAY_BUFFER, 8388608, g_tesselator.byte_buffer, GL_STREAM_DRAW);
        }

        if(g_tesselator.has_texture) {
            if(g_tesselator.use_vbo) {
                glTexCoordPointer(2, GL_FLOAT, 32, (void *)(3 * sizeof(float)));
            }else {
                glTexCoordPointer(2, GL_FLOAT, 32, &g_tesselator.byte_buffer[3 * sizeof(float)]);
            }
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        if(g_tesselator.has_color) {
            if(g_tesselator.use_vbo) {
                glColorPointer(4, GL_UNSIGNED_BYTE, 32, (void *)(5 * sizeof(float)));
            } else {
                glColorPointer(4, GL_UNSIGNED_BYTE, 32, &g_tesselator.byte_buffer[5 * sizeof(float)]);
            }
            glEnableClientState(GL_COLOR_ARRAY);
        }

        if(g_tesselator.use_vbo) {
            glVertexPointer(3, GL_FLOAT, 32, (void *)0);
        }else {
            glVertexPointer(3, GL_FLOAT, 32, g_tesselator.byte_buffer);
        }

        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(g_tesselator.draw_mode, GL_POINTS, g_tesselator.vertex_count);
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
    g_tesselator.vertex_count = 0;
    memset(g_tesselator.byte_buffer, 0, sizeof(g_tesselator.byte_buffer));
    g_tesselator.raw_buffer_index = 0;
    g_tesselator.added_vertices = 0;
}

void tesselator_begin_quads() {
    tesselator_begin(GL_QUADS);
}

void tesselator_begin(int mode) {
    if(g_tesselator.is_drawing) printf("Already tesselating!\n");
    g_tesselator.is_drawing = 1;
    tesselator_clear();
    g_tesselator.draw_mode = mode;
    g_tesselator.has_color = 0;
    g_tesselator.has_texture = 0;
    g_tesselator.no_color = 0;
}

void tesselator_color_float(float r, float g, float b, float a) {
    tesselator_color((uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255), (uint8_t)(a * 255));
}

void tesselator_color_opaque_int(uint32_t color) {
    tesselator_color_opaque(((color >> 24) & 0xFF), ((color >> 16) & 0xFF), ((color >> 8) & 0xFF));
}

void tesselator_color_opaque(uint8_t r, uint8_t g, uint8_t b) {
    tesselator_color(r, g, b, 255);
}

void tesselator_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if(g_tesselator.no_color) return;
    g_tesselator.has_color = 1;
    g_tesselator.color = (a << 24) | (b << 16) | (g << 8) | r;
}

void tesselator_vertex_uv(float x, float y, float z, float u, float v) {
    g_tesselator.has_texture = 1;
    g_tesselator.u = u;
    g_tesselator.v = v;
    tesselator_vertex(x, y, z);
}

void tesselator_vertex(float x, float y, float z) {
    g_tesselator.added_vertices++;

    if(g_tesselator.has_texture) {
        g_tesselator.raw_buffer[g_tesselator.raw_buffer_index + 3] = *(uint32_t *)&g_tesselator.u;
        g_tesselator.raw_buffer[g_tesselator.raw_buffer_index + 4] = *(uint32_t *)&g_tesselator.v;
    }

    if(g_tesselator.has_color) {
        g_tesselator.raw_buffer[g_tesselator.raw_buffer_index + 5] = g_tesselator.color;
    }

    g_tesselator.raw_buffer[g_tesselator.raw_buffer_index] = (int)(x + g_tesselator.x_offset);
    g_tesselator.raw_buffer[g_tesselator.raw_buffer_index + 1] = (int)(y + g_tesselator.y_offset);
    g_tesselator.raw_buffer[g_tesselator.raw_buffer_index + 2] = (int)(z + g_tesselator.z_offset);
    g_tesselator.raw_buffer_index += 8;
    g_tesselator.vertex_count++;

    if(g_tesselator.vertex_count % 4 == 0 && g_tesselator.raw_buffer_index >= 2097120) {
        tesselator_end();
    }
}

void tesselator_normal(float x, float y, float z) {
    glNormal3f(x, y, z);
}

void tesselator_disable_color() {
    g_tesselator.no_color = 1;
}

void tesselator_set_translation(double x, double y, double z) {
    g_tesselator.x_offset = x;
    g_tesselator.y_offset = y;
    g_tesselator.z_offset = z;
}