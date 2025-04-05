#include <gui/screen_error.h>

screen_t screen_error_create(char *title, char *text) {
    screen_t screen = screen_create();
    screen.type = SCREEN_TYPE_ERROR;
    screen.title = title;
    screen.text = text;
    
    screen.on_open = screen_error_on_open;
    screen.render = screen_error_render;
    screen.on_key_pressed = screen_error_on_key_pressed;

    return screen;
}

void screen_error_on_open(struct screen_s *proto_screen) {
    return;
}

void screen_error_render(struct screen_s *proto_screen, int mouse_x, int mouse_y) {
    screen_t *screen = (screen_t *)proto_screen;
    gui_fill_gradient(0, 0, screen->width, screen->height, 0x402020FF, 0x501010FF);
    gui_draw_centered_string(screen->font, screen->title, screen->width / 2, 90, 0xFFFFFFFF);
    gui_draw_centered_string(screen->font, screen->text, screen->width / 2, 110, 0xFFFFFFFF);

    screen_render(proto_screen, mouse_x, mouse_y);
}

void screen_error_on_key_pressed(struct screen_s *proto_screen, char event_char, int event_key) {
    return;
}