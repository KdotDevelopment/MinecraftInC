#include <gui/screen_options.h>
#include <gui/screen_controls.h>
#include <minecraft.h>

screen_t screen_options_create(screen_t *parent, game_settings_t *settings) {
    screen_t screen = screen_create();
    screen.type = SCREEN_TYPE_OPTIONS;
    screen.parent = (struct screen_s *)parent;
    screen.title = "Options";
    screen.settings = settings;
    //screen.grabs_mouse = 1;

    screen.on_open = screen_options_on_open;
    screen.on_button_clicked = screen_options_on_button_clicked;
    screen.render = screen_options_render;

    return screen;
}

void screen_options_on_open(struct screen_s *proto_screen) {
    screen_t *screen = (screen_t *)proto_screen;
    for(int i = 0; i < screen->settings->settings_count; i++) {
        button_t button = button_create_size(i, screen->width / 2 - 155 + i % 2 * 160, screen->height / 6 + 24 * (i / 2 + 1) - 24, 150, 20, game_settings_get_setting(screen->settings, i));
        screen->buttons = array_list_push(screen->buttons, &button);
    }
    button_t button_controls = button_create(100, screen->width / 2 - 100, screen->height / 6 + 132, "Controls...");
    button_t button_done = button_create(200, screen->width / 2 - 100, screen->height / 6 + 168, "Done");
    screen->buttons = array_list_push(screen->buttons, &button_controls);
    screen->buttons = array_list_push(screen->buttons, &button_done);
}

void screen_options_on_button_clicked(struct screen_s *proto_screen, button_t *button) {
    screen_t *screen = (screen_t *)proto_screen;
    minecraft_t *minecraft = (minecraft_t *)screen->minecraft;
    if(!button->active) return;
    if(button->id < 100) {
        game_settings_toggle_setting(((screen_t *)proto_screen)->settings, button->id);
        string_set(&button->string, game_settings_get_setting(screen->settings, button->id));
    }
    if(button->id == 100) {
        screen_t *controls = malloc(sizeof(screen_t));
        *controls = screen_controls_create(screen, &minecraft->settings);
        minecraft_set_current_screen(((screen_t *)proto_screen)->minecraft, (screen_t *)controls);
    }
    if(button->id == 200) {
        minecraft_set_current_screen(screen->minecraft, (screen_t *)screen->parent);
    }
}

void screen_options_render(struct screen_s *proto_screen, int mouse_x, int mouse_y) {
    screen_t *screen = (screen_t *)proto_screen;
    gui_fill_gradient(0, 0, screen->width, screen->height, 0x05050060, 0x303060A0);
    gui_draw_centered_string(screen->font, screen->title, screen->width / 2, 20, 0xFFFFFFFF);

    screen_render(proto_screen, mouse_x, mouse_y);
}