#include <gui/screen_pause.h>
#include <gui/screen_options.h>
#include <gui/screen_generate_world.h>
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
    screen->buttons = array_list_clear(screen->buttons);
    button_t button_options = button_create(0, screen->width / 2 - 100, screen->height / 4, "Options...");
    button_t button_change = button_create(1, screen->width / 2 - 100, screen->height / 4 + 24, "Change world...");
    button_t button_quit = button_create(2, screen->width / 2 - 100, screen->height / 4 + 48, "Quit game");
    button_t button_back = button_create(3, screen->width / 2 - 100, screen->height / 4 + 120, "Back to game");

    screen->buttons = array_list_push(screen->buttons, &button_options);
    screen->buttons = array_list_push(screen->buttons, &button_change);
    screen->buttons = array_list_push(screen->buttons, &button_quit);
    screen->buttons = array_list_push(screen->buttons, &button_back);
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
        //screen_t *generate_world = malloc(sizeof(screen_t));
        //*generate_world = screen_generate_world_create(screen);
        //minecraft_set_current_screen(screen->minecraft, (screen_t *)generate_world);
    }
    if(button->id == 2) {
        // close world
    }
    if(button->id == 3) {
        minecraft_grab_mouse(screen->minecraft);
        minecraft_set_current_screen(screen->minecraft, NULL);
    }
}

void screen_pause_render(struct screen_s *proto_screen, int mouse_x, int mouse_y, float partial_tick) {
    screen_t *screen = (screen_t *)proto_screen;
    gui_fill_gradient(0, 0, screen->width, screen->height, 0x05050060, 0x303060A0);
    gui_draw_centered_string(screen->font, "Game menu", screen->width / 2, 20, 0xFFFFFFFF);

    screen_render(proto_screen, mouse_x, mouse_y, partial_tick);
}