#include <gui/screen_pause.h>
#include <gui/screen_options.h>
#include <gui/screen_generate_level.h>
#include <gui/screen_save_level.h>
#include <gui/screen_load_level.h>
#include <minecraft.h>

screen_t screen_pause_create() {
    screen_t screen = screen_create();
    screen.type = SCREEN_TYPE_PAUSE;
    //screen.grabs_mouse = 1;

    screen.on_open = screen_pause_on_open;
    screen.on_button_clicked = screen_pause_on_button_clicked;
    screen.render = screen_pause_render;

    return screen;
}

void screen_pause_on_open(struct screen_s *proto_screen) {
    screen_t *screen = (screen_t *)proto_screen;
    button_t button_options = button_create(0, screen->width / 2 - 100, screen->height / 4, "Options...");
    button_t button_generate = button_create(1, screen->width / 2 - 100, screen->height / 4 + 24, "Generate new level...");
    button_t button_save = button_create(2, screen->width / 2 - 100, screen->height / 4 + 48, "Save level...");
    button_t button_load = button_create(3, screen->width / 2 - 100, screen->height / 4 + 72, "Load level...");
    button_t button_quit = button_create(4, screen->width / 2 - 100, screen->height / 4 + 120, "Back to game");

    button_save.active = 0;
    button_load.active = 0;

    screen->buttons = array_list_push(screen->buttons, &button_options);
    screen->buttons = array_list_push(screen->buttons, &button_generate);
    screen->buttons = array_list_push(screen->buttons, &button_save);
    screen->buttons = array_list_push(screen->buttons, &button_load);
    screen->buttons = array_list_push(screen->buttons, &button_quit);
}

void screen_pause_on_button_clicked(struct screen_s *proto_screen, button_t *button) {
    screen_t *screen = (screen_t *)proto_screen;
    if(button->active == 0) return;
    if(button->id == 0) {
        screen_t *options = malloc(sizeof(screen_t));
        *options = screen_options_create(screen, &screen->minecraft->settings);
        minecraft_set_current_screen(screen->minecraft, (screen_t *)options);
    }
    if(button->id == 1) {
        screen_t *generate_level = malloc(sizeof(screen_t));
        *generate_level = screen_generate_level_create(screen);
        minecraft_set_current_screen(screen->minecraft, (screen_t *)generate_level);
    }
    if(button->id == 2) {
        screen_t *save_level = malloc(sizeof(screen_t));
        *save_level = screen_save_level_create(screen);
        minecraft_set_current_screen(screen->minecraft, (screen_t *)save_level);
    }
    if(button->id == 3) {
        screen_t *load_level = malloc(sizeof(screen_t));
        *load_level = screen_load_level_create(screen);
        minecraft_set_current_screen(screen->minecraft, (screen_t *)load_level);
    }
    if(button->id == 4) {
        minecraft_grab_mouse(screen->minecraft);
        minecraft_set_current_screen(screen->minecraft, NULL);
    }
}

void screen_pause_render(struct screen_s *proto_screen, int mouse_x, int mouse_y) {
    screen_t *screen = (screen_t *)proto_screen;
    gui_fill_gradient(0, 0, screen->width, screen->height, 0x05050060, 0x303060A0);
    gui_draw_centered_string(screen->font, "Game menu", screen->width / 2, 20, 0xFFFFFFFF);

    screen_render(proto_screen, mouse_x, mouse_y);
}