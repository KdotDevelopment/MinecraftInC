#include <gui/progress_bar.h>

#include <renderer/tesselator.h>
#include <minecraft.h>

#include <util/time.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <stdio.h>
#include <zlib.h>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

int fileno(FILE *stream);

progress_bar_t progress_bar_create(struct minecraft_s *minecraft) {
    progress_bar_t bar = { 0 };
    bar.minecraft = minecraft;
    bar.text = "";
    bar.title = "";
    bar.start = time_millis();
    return bar;
}

void progress_bar_set_title(progress_bar_t *bar, char *title) {
    bar->title = title;
    int a1 = bar->minecraft->width * 240 / bar->minecraft->height;
    int a2 = bar->minecraft->height * 240 / bar->minecraft->height;
    glClear(GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, a1, a2, 0.0, 100.0, 300.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -200.0);
}

void progress_bar_set_text(progress_bar_t *bar, char *text) {
    bar->text = text;
    progress_bar_set_progress(bar, -1);
}

void progress_bar_set_progress(progress_bar_t *bar, int progress) {
    int64_t time = time_millis();
    if(time - bar->start < 0 || time - bar->start >= 20) {
        bar->start = time;
        int a1 = bar->minecraft->width * 240 / bar->minecraft->height;
        int a2 = bar->minecraft->height * 240 / bar->minecraft->height;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, textures_load(&bar->minecraft->textures, "dirt.png"));
        tesselator_begin_quads();
        tesselator_color_opaque_int(0x404040ff);
        tesselator_vertex_uv(0.0, a2, 0.0, 0.0, a2 / 32.0);
        tesselator_vertex_uv(a1, a2, 0.0, a1 / 32.0, a2 / 32.0);
        tesselator_vertex_uv(a1, 0.0, 0.0, a1 / 32.0, 0.0);
        tesselator_vertex_uv(0.0, 0.0, 0.0, 0.0, 0.0);
        tesselator_end();
        if(progress >= 0) {
            int b1 = a1 / 2 - 50;
            int b2 = a2 / 2 + 16;
            glDisable(GL_TEXTURE_2D);
            tesselator_begin_quads();
            tesselator_color_opaque_int(0x808080ff);
            tesselator_vertex(b1, b2, 0.0);
            tesselator_vertex(b1, b2 + 2, 0.0);
            tesselator_vertex(b1 + 100, b2 + 2, 0.0);
            tesselator_vertex(b1 + 100, b2, 0.0);
            tesselator_color_opaque_int(0x80ff80ff);
            tesselator_vertex(b1, b2, 0.0);
            tesselator_vertex(b1, b2 + 2, 0.0);
            tesselator_vertex(b1 + progress, b2 + 2, 0.0);
            tesselator_vertex(b1 + progress, b2, 0.0);
            tesselator_end();
            glEnable(GL_TEXTURE_2D);
        }
        
        font_render(&bar->minecraft->font, bar->title, (a1 - font_get_width(&bar->minecraft->font, bar->title)) / 2, a2 / 2 - 4 - 16, 0xFFFFFFFF);
        font_render(&bar->minecraft->font, bar->text, (a1 - font_get_width(&bar->minecraft->font, bar->text)) / 2, a2 / 2 - 4 + 8, 0xFFFFFFFF);
        
        while(SDL_PollEvent(&(SDL_Event){ 0 }));
        SDL_GL_SwapWindow(bar->minecraft->window);
    }
}

nbt_base_t progress_bar_read(FILE *chunk_file) {
    gzFile gz_input = gzdopen(fileno(chunk_file), "rb");
    if(!gz_input) {
        printf("Could not open dat file for reading.\n");
        return (nbt_base_t){ .null = 1 };
    }

    nbt_base_t nbt = nbt_read_named_tag(gz_input);

    gzclose(gz_input);

    return nbt;
}

void progress_bar_write(FILE *chunk_file, nbt_base_t *nbt) {
    gzFile gz_output = gzdopen(fileno(chunk_file), "wb");
    if(!gz_output) {
        printf("Could not open dat file for writing.\n");
        return;
    }

    nbt_write_named_tag(gz_output, nbt);

    gzclose(gz_output);
}