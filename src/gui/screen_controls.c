#include <gui/screen_controls.h>
#include <gui/gui.h>
#include <minecraft.h>

#include <util/array_list.h>
#include <util/string.h>

screen_t screen_controls_create(screen_t *parent, game_settings_t *settings) {
    screen_t screen = screen_create();
    screen.type = SCREEN_TYPE_CONTROLS;
    screen.settings = settings;
    screen.parent = (struct screen_s *)parent;
    screen.id = -1; // AKA selected
    //screen.grabs_mouse = 1;
    screen.title = "Controls";

    screen.on_open = screen_controls_on_open;
    screen.render = screen_controls_render;
    screen.on_key_pressed = screen_controls_on_key_pressed;
    screen.on_button_clicked = screen_controls_on_button_clicked;

    return screen;
}

void screen_controls_on_open(struct screen_s *proto_screen) {
    screen_t *screen = (screen_t *)proto_screen;
    for(int i = 0; i < array_list_length(screen->settings->bindings); i++) {
        char *text = game_settings_get_binding(screen->settings, i);
        button_t button = button_create_size(i, screen->width / 2 - 155 + i % 2 * 160, screen->height / 6 + 24 * (i / 2 + 1) - 24, 150, 20, text);
        screen->buttons = array_list_push(screen->buttons, &button);
        string_free(text);
    }
    button_t button = button_create(200, screen->width / 2 - 100, screen->height / 6 + 168, "Done");
    screen->buttons = array_list_push(screen->buttons, &button);

    screen_on_open(proto_screen);
}

void screen_controls_render(struct screen_s *proto_screen, int mx, int my) {
    screen_t *screen = (screen_t *)proto_screen;
    gui_fill_gradient(0, 0, screen->width, screen->height, 0x05050060, 0x303060A0);
    gui_draw_centered_string(screen->font, screen->title, screen->width / 2, 20, 0xFFFFFFFF);

    screen_render(proto_screen, mx, my);
}

void screen_controls_on_key_pressed(struct screen_s *proto_screen, char event_char, int event_key) {
    screen_t *screen = (screen_t *)proto_screen;
    if(screen->id >= 0) {
        game_settings_set_binding(screen->settings, screen->id, event_key);
        char *text = game_settings_get_binding(screen->settings, screen->id);
        string_set(&screen->buttons[screen->id].string, text);
        screen->id = -1;
        string_free(text);
    }else {
        screen->type = SCREEN_TYPE_NONE;
        screen_on_key_pressed(proto_screen, event_char, event_key);
        screen->type = SCREEN_TYPE_CONTROLS;
    }
}

void screen_controls_on_button_clicked(struct screen_s *proto_screen, button_t *button) {
    screen_t *screen = (screen_t *)proto_screen;
    for(int i = 0; i < array_list_length(screen->settings->bindings); i++) {
        char *text = game_settings_get_binding(screen->settings, i);
        string_set(&screen->buttons[i].string, text);
        string_free(text);
    }

    if(button->id == 200) {
        minecraft_set_current_screen(screen->minecraft, (screen_t *)screen->parent);
    }else {
        screen->id = button->id;
        char *text = game_settings_get_binding(screen->settings, button->id);
        string_concat_front("> ", &text);
        string_concat(&text, " <");
        string_set(&button->string, text);
        string_free(text);
    }
}