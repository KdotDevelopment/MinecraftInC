#include <gui/gui.h>

#include <renderer/tesselator.h>
#include <gui/font.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

void gui_fill(int x0, int y0, int x1, int y1, uint32_t color) {
    float red = (float)(color >> 24) / 255.0;
    float green = (float)(color >> 16 & 0xFF) / 255.0;
    float blue = (float)(color >> 8 & 0xFF) / 255.0;
    float alpha = (float)(color & 0xFF) / 255.0;
    tesselator_begin(&g_tesselator);
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(red, green, blue, alpha);
    tesselator_vertex(x0, y1, 0);
    tesselator_vertex(x1, y1, 0);
    tesselator_vertex(x1, y0, 0);
    tesselator_vertex(x0, y0, 0);
    tesselator_end();
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void gui_fill_gradient(int x0, int y0, int x1, int y1, uint32_t color0, uint32_t color1) {
    float r0 = (float)(color0 >> 24) / 255.0;
    float g0 = (float)(color0 >> 16 & 0xFF) / 255.0;
    float b0 = (float)(color0 >> 8 & 0xFF) / 255.0;
    float a0 = (float)(color0 & 0xFF) / 255.0;
    float r1 = (float)(color1 >> 24) / 255.0;
    float g1 = (float)(color1 >> 16 & 0xFF) / 255.0;
    float b1 = (float)(color1 >> 8 & 0xFF) / 255.0;
    float a1 = (float)(color1 & 0xFF) / 255.0;
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_QUADS);
    glColor4f(r0, g0, b0, a0);
    glVertex2f((float)x1, (float)y0);
    glVertex2f((float)x0, (float)y0);
    glColor4f(r1, g1, b1, a1);
    glVertex2f((float)x0, (float)y1);
    glVertex2f((float)x1, (float)y1);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void gui_draw_centered_string(font_t *font, char *string, int x, int y, uint32_t color) {
    font_render(font, string, x - font_get_width(font, string) / 2, y, color);
}

void gui_draw_string(font_t *font, char *string, int x, int y, uint32_t color) {
    font_render(font, string, x, y, color);
}

void gui_blit(int x0, int y0, int width, int height, int u, int v, float blit_offset) {
    float s = 0.00390625 - 0.00000001;
    tesselator_begin();
    tesselator_vertex_uv(x0, y0 + v, blit_offset, width * s, (height + v) * s);
    tesselator_vertex_uv(x0 + u, y0 + v, blit_offset, (width + u) * s, (height + v) * s);
    tesselator_vertex_uv(x0 + u, y0, blit_offset, (width + u) * s, height * s);
    tesselator_vertex_uv(x0, y0, blit_offset, width * s, height * s);
    tesselator_end();
}