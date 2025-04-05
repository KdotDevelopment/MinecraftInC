#include <gui/screen_death.h>
#include <gui/screen_generate_level.h>
#include <gui/screen_load_level.h>
#include <minecraft.h>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

screen_t screen_death_create() {
    screen_t screen = screen_create();

    screen.type = SCREEN_TYPE_DEATH;
    screen.on_open = screen_death_on_open;
    screen.on_button_clicked = screen_death_on_button_clicked;
    screen.render = screen_death_render;

    return screen;
}

void screen_death_on_open(struct screen_s *screen) {
    screen->buttons = array_list_clear(screen->buttons);
    button_t button_respawn = button_create(0, screen->width / 2 - 100, screen->height / 4 + 72, "Generate new level...");
    button_t button_load = button_create(1, screen->width / 2 - 100, screen->height / 4 + 96, "Load level...");
    button_load.active = 0;
    screen->buttons = array_list_push(screen->buttons, &button_respawn);
    screen->buttons = array_list_push(screen->buttons, &button_load);
}

void screen_death_on_button_clicked(struct screen_s *screen, button_t *button) {
    if(button->id == 0) {
        screen_t *gen_screen = malloc(sizeof(screen_t));
        *gen_screen = screen_generate_level_create(screen);
        minecraft_set_current_screen(screen->minecraft, gen_screen);
    }else if(button->id == 1) {
        screen_t *load_screen = malloc(sizeof(screen_t));
        *load_screen = screen_load_level_create(screen);
        minecraft_set_current_screen(screen->minecraft, load_screen);
    }
}

void screen_death_render(struct screen_s *screen, int mx, int my) {
    gui_fill_gradient(0, 0, screen->width, screen->height, 0x50000060, 0x803030A0);
    glPushMatrix();
    glScalef(2, 2, 2);
    gui_draw_centered_string(screen->font, "Game Over!", screen->width / 2 / 2, 30, 0xFFFFFFFF);
    glPopMatrix();
    gui_draw_centered_string(screen->font, "Score: &e", screen->width / 2, 100, 0xFFFFFFFF);
    gui_fill_gradient(0, 0, screen->width, screen->height, 0x60500000, 0xA0803030);
    screen_render(screen, mx, my);
}
