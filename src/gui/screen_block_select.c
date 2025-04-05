#include <gui/screen_block_select.h>
#include <minecraft.h>
#include <session_data.h>
#include <gui/gui.h>
#include <renderer/tesselator.h>

#include <util/array_list.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

screen_t screen_block_select_create() {
    screen_t screen = screen_create();
    screen.grabs_mouse = 1;
    screen.type = SCREEN_TYPE_BLOCK_SELECT;
    screen.render = screen_block_select_render;
    screen.on_mouse_clicked = screen_block_select_on_mouse_clicked;

    return screen;
}

int get_block_on_screen(screen_t *screen, int mx, int my) {
    for(int i = 0; i < array_list_length(session_allowed_blocks); i++) {
        int x = screen->width / 2 + i % 9 * 24 + -108 - 3;
        int y = screen->height / 2 + i / 9 * 24 + -60 + 3;
        if(mx >= x && mx <= x + 24 && my >= y - 12 && my <= y + 12) return i;
    }
    return -1;
}

void screen_block_select_render(struct screen_s *proto_screen, int mx, int my) {
    screen_t *screen = (screen_t *)proto_screen;
    int block_i = get_block_on_screen(screen, mx, my);
    gui_fill_gradient(screen->width / 2 - 120, 30, screen->width / 2 + 120, 180, 0x05050090, 0x303060C0);
    if(block_i >= 0) {
        int x = screen->width / 2 + block_i % 9 * 24 + -108;
        int y = screen->height / 2 + block_i / 9 * 24 + -60;
        gui_fill_gradient(x - 3, y - 8, x + 23, y + 18, 0xFFFFFF90, 0xFFFFFFC0);
    }
    gui_draw_centered_string(screen->font, "Select block", screen->width / 2, 40, 0xFFFFFFFF);
    glBindTexture(GL_TEXTURE_2D, textures_load(&((minecraft_t *)screen->minecraft)->textures, "terrain.png"));
    for(int i = 0; i < array_list_length(session_allowed_blocks); i++) {
        uint8_t block_id = session_allowed_blocks[i];
        block_t *block = &block_list[block_id];
        glPushMatrix();
        int x = screen->width / 2 + i % 9 * 24 + -108;
        int y = screen->height / 2 + i / 9 * 24 + -60;
        glTranslatef(x, y, 0.0);
        glScalef(10.0, 10.0, 10.0);
        glTranslatef(1.0, 0.5, 8.0);
        glRotatef(-30.0, 1.0, 0.0, 0.0);
        glRotatef(45.0, 0.0, 1.0, 0.0);
        if(block_i == i) glScalef(1.6, 1.6, 1.6);
        glTranslatef(-1.5, 0.5, 0.5);
        glScalef(-1.0, -1.0, -1.0);
        tesselator_begin();
        block->render_full_brightness(block);
        tesselator_end();
        glPopMatrix();
    }

    screen_render(proto_screen, mx, my);
}

void screen_block_select_on_mouse_clicked(struct screen_s *proto_screen, int x, int y, int button) {
    screen_t *screen = (screen_t *)proto_screen;
    if(button == SDL_BUTTON_LEFT) {
        inventory_replace_slot(&((minecraft_t *)screen->minecraft)->player.inventory, get_block_on_screen(screen, x, y));
        minecraft_set_current_screen(screen->minecraft, NULL);
    }

    screen_on_mouse_clicked(proto_screen, x, y, button);
}