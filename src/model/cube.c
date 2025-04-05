#include <model/cube.h>
#include <model/vec3.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

#include <string.h>

cube_t cube_create(int x_texture_offset, int y_texture_offset) {
    cube_t cube;
    memset(&cube, 0, sizeof(cube_t));
    cube.visible = 1;
    cube.x_texture_offset = x_texture_offset;
    cube.y_texture_offset = y_texture_offset;
    return cube;
}

void cube_add_box(cube_t *cube, float x, float y, float z, int w, int h, int d, float g) {
    float end_x = x + w;
    float end_y = y + h;
    float end_z = z + d;
    x -= g;
    y -= g;
    z -= g;
    end_x += g;
    end_y += g;
    end_z += g;
    if(cube->mirror) {
        g = end_x;
        end_x = x;
        x = g;
    }

    vertex_t v1 = (vertex_t){ (vec3_t){ x, y, z }, 0, 0 };
    vertex_t v2 = (vertex_t){ (vec3_t){ end_x, y, z }, 0, 8 };
    vertex_t v3 = (vertex_t){ (vec3_t){ end_x, end_y, z }, 8, 8 };
    vertex_t v4 = (vertex_t){ (vec3_t){ x, end_y, z }, 8, 0 };
    vertex_t v5 = (vertex_t){ (vec3_t){ x, y, end_z }, 0, 0 };
    vertex_t v6 = (vertex_t){ (vec3_t){ end_x, y, end_z }, 0, 8 };
    vertex_t v7 = (vertex_t){ (vec3_t){ end_x, end_y, end_z }, 8, 8 };
    vertex_t v8 = (vertex_t){ (vec3_t){ x, end_y, end_z }, 8, 0 };

    cube->vertices[0] = v1;
    cube->vertices[1] = v2;
    cube->vertices[2] = v3;
    cube->vertices[3] = v4;
    cube->vertices[4] = v5;
    cube->vertices[5] = v6;
    cube->vertices[6] = v7;
    cube->vertices[7] = v8;
    cube->polygons[0] = polygon_create((vertex_t[]){ v6, v2, v3, v7 }, cube->x_texture_offset + d + w,     cube->y_texture_offset + d, cube->x_texture_offset + d + w + d,     cube->y_texture_offset + d + h);
    cube->polygons[1] = polygon_create((vertex_t[]){ v1, v5, v8, v4 }, cube->x_texture_offset,             cube->y_texture_offset + d, cube->x_texture_offset + d,             cube->y_texture_offset + d + h);
    cube->polygons[2] = polygon_create((vertex_t[]){ v6, v5, v1, v2 }, cube->x_texture_offset + d,         cube->y_texture_offset,     cube->x_texture_offset + d + w,         cube->y_texture_offset + d);
    cube->polygons[3] = polygon_create((vertex_t[]){ v3, v4, v8, v7 }, cube->x_texture_offset + d + w,     cube->y_texture_offset,     cube->x_texture_offset + d + w + w,     cube->y_texture_offset + d);
    cube->polygons[4] = polygon_create((vertex_t[]){ v2, v1, v4, v3 }, cube->x_texture_offset + d,         cube->y_texture_offset + d, cube->x_texture_offset + d + w,         cube->y_texture_offset + d + h);
    cube->polygons[5] = polygon_create((vertex_t[]){ v5, v6, v7, v8 }, cube->x_texture_offset + d + w + d, cube->y_texture_offset + d, cube->x_texture_offset + d + w + d + w, cube->y_texture_offset + d + h);

    if(cube->mirror) {
        for(uint8_t i = 0; i < 6; i++) { // 6 = 6 polygons per cube
            polygon_flip(&cube->polygons[i]);
        }
    }
}

void cube_set_pos(cube_t *cube, float x, float y, float z) {
    cube->x = x;
    cube->y = y;
    cube->z = z;
}

void cube_render(cube_t *cube, float scale) {
    if(!cube->visible) return;
    if(!cube->compiled) cube_compile(cube, scale);

    if(cube->x_rot != 0 || cube->y_rot != 0 || cube->z_rot != 0) {
        glPushMatrix();
        glTranslatef(cube->x * scale, cube->y * scale, cube->z * scale);
        if(cube->z_rot != 0) glRotatef(cube->z_rot * (180.0 / PI), 0, 0, 1);
        if(cube->y_rot != 0) glRotatef(cube->y_rot * (180.0 / PI), 0, 1, 0);
        if(cube->x_rot != 0) glRotatef(cube->x_rot * (180.0 / PI), 1, 0, 0);
        glCallList(cube->list);
        glPopMatrix();
    }else if(cube->x == 0 && cube->y == 0 && cube->z == 0) {
        glCallList(cube->list);
    }else {
        glTranslatef(cube->x * scale, cube->y * scale, cube->z * scale);
        glCallList(cube->list);
        glTranslatef(-cube->x * scale, -cube->y * scale, -cube->z * scale);
    }
}

void cube_compile(cube_t *cube, float scale) {
    cube->list = glGenLists(1);
    glNewList(cube->list, GL_COMPILE);
    glBegin(GL_QUADS);

    for(int i = 0; i < 6; i++) { // 6 = cube polygon length
        polygon_t p1 = cube->polygons[i];
        polygon_t p2 = p1;

        vec3_t n1 = vec3_normalize(vec3_subtract(p1.vertices[1].pos, p2.vertices[0].pos));
        vec3_t n2 = vec3_normalize(vec3_subtract(p2.vertices[2].pos, p2.vertices[0].pos));
        vec3_t normal = vec3_normalize((vec3_t){n1.y * n2.z - n1.z * n2.y, n1.z * n2.x - n1.x * n2.z, n1.x * n2.y - n1.y * n2.x});

        glNormal3f(normal.x, normal.y, normal.z);

        for(int j = 0; j < 4; j++) {
            vertex_t vertex = p2.vertices[j];
            glTexCoord2f(vertex.u, vertex.v);
            glVertex3f(vertex.pos.x * scale, vertex.pos.y * scale, vertex.pos.z * scale);
        }
    }

    glEnd();
    glEndList();
    cube->compiled = 1;
}